#include "ifs.h"
#include "../memory/memory.h"
#include "../hardware/display.h"
#include "../hardware/disk.h"
#include "../lib/string.h"
#include "../hardware/timer.h"
#include "../process/task.h"

ifs_open_file_t open_files[MAX_OPEN_FILES] = {0}; // array definition w/ zero-initialization
ifs_superblock_t superblock;

#define IFS_BLOCK_BITMAP_OFFSET 2048  // Assume bitmap starts at 2KB
#define IFS_TOTAL_BLOCKS 8192        // Total number of blocks

static uint8_t block_bitmap[IFS_TOTAL_BLOCKS / 8];  // 1 bit per block
// **Load block bitmap from disk**
void ifs_load_block_bitmap() {
    disk_read(IFS_DISK, IFS_BLOCK_BITMAP_OFFSET, block_bitmap, sizeof(block_bitmap));
}

// **Allocate a block**
uint64_t allocate_block() {
    for (uint64_t i = 0; i < IFS_TOTAL_BLOCKS; i++) {
        uint64_t byte_index = i / 8;
        uint64_t bit_index = i % 8;

        if (!(block_bitmap[byte_index] & (1 << bit_index))) {  // Block is free
            block_bitmap[byte_index] |= (1 << bit_index);  // Mark as used
            disk_write(IFS_DISK, IFS_BLOCK_BITMAP_OFFSET, block_bitmap, sizeof(block_bitmap));  // Update bitmap on disk
            return i;
        }
    }

    display_write("[IFS] No free blocks available\n");
    return -1;  // No free blocks
}

// **Free a block**
void free_block(uint64_t block_id) {
    if (block_id >= IFS_TOTAL_BLOCKS) return;

    uint64_t byte_index = block_id / 8;
    uint64_t bit_index = block_id % 8;

    block_bitmap[byte_index] &= ~(1 << bit_index);  // Mark as free
    disk_write(IFS_DISK, IFS_BLOCK_BITMAP_OFFSET, block_bitmap, sizeof(block_bitmap));  // Update bitmap on disk
}

// Create a new file
int ifs_create_file(const char *path, uint16_t mode) {
    if (!path || !*path) return -1;

    // Check if file already exists
    if (ifs_find_inode(path) != -1) {
        display_write("[IFS] File already exists\n");
        return -1;
    }

    // Allocate a new inode
    int inode_id = ifs_allocate_inode();
    if (inode_id == -1) {
        display_write("[IFS] No free inode available\n");
        return -1;
    }

    // Initialize inode
    ifs_inode_t *inode = ifs_get_inode(inode_id);
    memset(inode, 0, sizeof(ifs_inode_t));
    inode->type = FILE_TYPE_REGULAR;
    inode->permissions = mode;
    inode->size = 0;
    inode->created_time = get_system_ticks();
    inode->modified_time = inode->created_time;
    inode->accessed_time = inode->created_time;

    // Write inode to disk
    ifs_update_inode(inode_id);

    // Map the filename to the inode
    if (!add_directory_entry(path, inode_id)) {
        display_write("[IFS] Failed to add directory entry\n");
        return -1;
    }

    return 0;
}

// Delete a file
int ifs_delete_file(const char *path) {
    if (!path || !*path) return -1;

    // Get inode ID
    int inode_id = ifs_find_inode(path);
    if (inode_id == -1) {
        display_write("[IFS] File not found\n");
        return -1;
    }

    // Remove from directory
    if (!remove_directory_entry(path)) {
        display_write("[IFS] Failed to remove directory entry\n");
        return -1;
    }

    // Free the inode
    ifs_delete_inode(inode_id);

    return 0;
}

// Find an inode by file path
int ifs_find_inode(const char *path) {
    return find_directory_entry(path);
}

// Read a file into a buffer
int ifs_read_file(const char *path, void *buffer, size_t size) {
    int inode_id = ifs_find_inode(path);
    if (inode_id == -1) {
        display_write("[IFS] File not found\n");
        return -1;
    }

    ifs_inode_t *inode = ifs_get_inode(inode_id);
    if (!inode || inode->size == 0) {
        return -1;
    }

    // Check read permission
    if (ifs_check_permissions(inode, IFS_PERM_READ) != 0) {
        display_write("[IFS] Permission denied\n");
        return -1;
        }

    size_t bytes_read = 0;
    uint64_t block_index = 0;
    
    while (bytes_read < size && block_index < MAX_BLOCKS) {
        if (inode->block_pointers[block_index] == 0) break;  // No more data

        size_t read_size = ((size - bytes_read) > (inode->size - bytes_read)) 
                           ? (inode->size - bytes_read) 
                           : (size - bytes_read);

        if (!disk_read(IFS_DISK, inode->block_pointers[block_index], 
                       (uint8_t*)buffer + bytes_read, read_size)) {
            display_write("[IFS] Read error\n");
            return bytes_read > 0 ? bytes_read : -1;  // Return what was read, or error
        }

        bytes_read += read_size;
        block_index++;
    }

    inode->accessed_time = get_system_ticks();
    ifs_update_inode(inode_id);

    return bytes_read;  // Return total bytes read
}



// Write to a file
int ifs_write_file(const char *path, const void *buffer, size_t size) {
    int inode_id = ifs_find_inode(path);
    if (inode_id == -1) {
        display_write("[IFS] File not found\n");
        return -1;
    }

    ifs_inode_t *inode = ifs_get_inode(inode_id);
    if (!inode) return -1;

    // Check write permission
    if (ifs_check_permissions(inode, IFS_PERM_WRITE) != 0) {
        display_write("[IFS] Permission denied\n");
        return -1;
    }

    size_t bytes_written = 0;
    uint64_t block_index = 0;

    while (bytes_written < size) {
        if (block_index >= MAX_BLOCKS) {
            display_write("[IFS] File exceeds max block allocation\n");
            return bytes_written > 0 ? bytes_written : -1;
        }

        if (inode->block_pointers[block_index] == 0) {
            inode->block_pointers[block_index] = allocate_block();
            if (inode->block_pointers[block_index] == -1) {
                display_write("[IFS] No free blocks available\n");
                return bytes_written > 0 ? bytes_written : -1;
            }
        }

        size_t write_size = (size - bytes_written) > (superblock.block_size)
                            ? (superblock.block_size)
                            : (size - bytes_written);

        if (!disk_write(IFS_DISK, inode->block_pointers[block_index], 
                        (const uint8_t*)buffer + bytes_written, write_size)) {
            display_write("[IFS] Write error\n");
            return bytes_written > 0 ? bytes_written : -1;
        }

        bytes_written += write_size;
        block_index++;
    }

    inode->size += bytes_written;
    inode->modified_time = get_system_ticks();
    ifs_update_inode(inode_id);

    return bytes_written;
}



// For Opening / Closing a file
int ifs_open(const char *path, uint32_t flags) {
    int inode_id = ifs_find_inode(path);
    if (inode_id == -1) {
        display_write("[IFS] File not found\n");
        return -1;
    }

    ifs_inode_t *inode = ifs_get_inode(inode_id);
    if (!inode) return -1;

    // Check if the file is already open
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (open_files[i].inode_id == inode_id && open_files[i].fd != 0) {
            display_write("[IFS] File already open\n");
            return open_files[i].fd;
        }
    }

    // Find free file descriptor slot
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (open_files[i].fd == 0) {  // Empty slot
            open_files[i].fd = i + 1;
            open_files[i].inode_id = inode_id;
            open_files[i].position = 0;
            open_files[i].flags = flags;
            return open_files[i].fd;
        }
    }

    display_write("[IFS] No available file descriptors\n");
    return -1;
}

int ifs_close(int fd) {
    if (fd <= 0 || fd > MAX_OPEN_FILES || open_files[fd - 1].fd == 0) {
        display_write("[IFS] Invalid file descriptor\n");
        return -1;
    }

    open_files[fd - 1].fd = 0;  // Mark slot as free
    open_files[fd - 1].inode_id = 0;
    open_files[fd - 1].position = 0;
    open_files[fd - 1].flags = 0;

    return 0;
}


int ifs_check_permissions(ifs_inode_t *inode, uint32_t requested_perm) {
    if (!inode) return -1;

    uint32_t user = task_current()->uid;  // Get the current user ID
    uint32_t group = task_current()->gid; // Get the current group ID

    uint32_t user_perm = IFS_PERM_USER(inode->permissions);
    uint32_t group_perm = IFS_PERM_GROUP(inode->permissions);
    uint32_t other_perm = IFS_PERM_OTHER(inode->permissions);

    if (inode->uid == user && (user_perm & requested_perm)) return 0;  // User has permission
    if (inode->gid == group && (group_perm & requested_perm)) return 0; // Group has permission
    if (other_perm & requested_perm) return 0;  // Others have permission

    return -1; // Permission denied
}

int ifs_chmod(const char *path, uint32_t new_permissions) {
    int inode_id = ifs_find_inode(path);
    if (inode_id == -1) {
        display_write("[IFS] File not found\n");
        return -1;
    }

    ifs_inode_t *inode = ifs_get_inode(inode_id);
    if (!inode) return -1;

    // Only the owner can change permissions
    if (task_current()->uid != inode->uid) {
        display_write("[IFS] Permission denied\n");
        return -1;
    }

    inode->permissions = new_permissions;
    ifs_update_inode(inode_id);
    return 0;
}

int ifs_chown(const char *path, uint32_t new_uid, uint32_t new_gid) {
    int inode_id = ifs_find_inode(path);
    if (inode_id == -1) {
        display_write("[IFS] File not found\n");
        return -1;
    }

    ifs_inode_t *inode = ifs_get_inode(inode_id);
    if (!inode) return -1;

    // Only the superuser (UID 0) can change ownership
    if (task_current()->uid != 0) {
        display_write("[IFS] Permission denied\n");
        return -1;
    }

    inode->uid = new_uid;
    inode->gid = new_gid;
    ifs_update_inode(inode_id);
    return 0;
}



