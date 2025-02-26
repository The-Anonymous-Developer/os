#include "ifs.h"
#include "../hardware/display.h"

int add_directory_entry(const char* path, uint64_t inode_id) {
    display_write("[IFS] add_directory_entry() not implemented yet!\n");
    return 1; // Stub: Simulate success
}

int remove_directory_entry(const char* path) {
    display_write("[IFS] remove_directory_entry() not implemented yet!\n");
    return 1; // Stub: Simulate success
}

int find_directory_entry(const char* path) {
    display_write("[IFS] find_directory_entry() not implemented yet!\n");
    return -1; // Stub: Simulate "not found"
}


int ifs_create_directory(const char *path) {
    if (!path || !*path) return -1;

    // Ensure directory does not already exist
    if (ifs_find_inode(path) != -1) {
        display_write("[IFS] Directory already exists\n");
        return -1;
    }

    // Allocate an inode for the directory
    int inode_id = ifs_allocate_inode();
    if (inode_id == -1) {
        display_write("[IFS] No free inode available\n");
        return -1;
    }

    // Initialize directory inode
    ifs_inode_t *inode = ifs_get_inode(inode_id);
    memset(inode, 0, sizeof(ifs_inode_t));
    inode->type = FILE_TYPE_DIRECTORY;
    inode->size = 0;
    inode->created_time = get_system_ticks();
    inode->modified_time = inode->created_time;
    inode->accessed_time = inode->created_time;

    // Allocate a block for directory entries
    inode->block_pointers[0] = allocate_block();
    if (inode->block_pointers[0] == (uint64_t)-1) {
        display_write("[IFS] Failed to allocate block for directory\n");
        ifs_delete_inode(inode_id);
        return -1;
    }

    // Write inode to disk
    ifs_update_inode(inode_id);

    // Add to parent directory
    if (!add_directory_entry(path, inode_id)) {
        display_write("[IFS] Failed to add directory entry\n");
        return -1;
    }

    return 0;
}


int ifs_delete_directory(const char *path) {
    if (!path || !*path) return -1;

    // Get inode ID
    int inode_id = ifs_find_inode(path);
    if (inode_id == -1) {
        display_write("[IFS] Directory not found\n");
        return -1;
    }

    ifs_inode_t *inode = ifs_get_inode(inode_id);
    if (!inode || inode->type != FILE_TYPE_DIRECTORY) {
        display_write("[IFS] Not a directory\n");
        return -1;
    }

    // Ensure directory is empty before deleting
    ifs_dir_entry_t entries[IFS_MAX_DIRECTORY_ENTRIES];
    if (disk_read(IFS_DISK, inode->block_pointers[0], entries, sizeof(entries))) {
        for (int i = 0; i < IFS_MAX_DIRECTORY_ENTRIES; i++) {
            if (entries[i].inode_id != 0) {
                display_write("[IFS] Directory is not empty\n");
                return -1;
            }
        }
    }

    // Remove from parent directory
    if (!remove_directory_entry(path)) {
        display_write("[IFS] Failed to remove directory entry\n");
        return -1;
    }

    // Free inode and block
    free_block(inode->block_pointers[0]);
    ifs_delete_inode(inode_id);

    return 0;
}


int ifs_list_directory(const char *path) {
    int inode_id = ifs_find_inode(path);
    if (inode_id == -1) {
        display_write("[IFS] Directory not found\n");
        return -1;
    }

    ifs_inode_t *inode = ifs_get_inode(inode_id);
    if (!inode || inode->type != FILE_TYPE_DIRECTORY) {
        display_write("[IFS] Not a directory\n");
        return -1;
    }

    ifs_dir_entry_t entries[IFS_MAX_DIRECTORY_ENTRIES];
    if (!disk_read(IFS_DISK, inode->block_pointers[0], entries, sizeof(entries))) {
        display_write("[IFS] Failed to read directory\n");
        return -1;
    }

    display_write("[IFS] Directory contents:\n");
    for (int i = 0; i < IFS_MAX_DIRECTORY_ENTRIES; i++) {
        if (entries[i].inode_id != 0) {
            display_write(entries[i].name);
            display_write("\n");
        }
    }

    return 0;
}


int ifs_resolve_path(const char *path) {
    char temp[IFS_MAX_FILENAME_LENGTH];
    strncpy(temp, path, sizeof(temp));

    char *token = strtok_custom(temp, "/");
    int parent_inode = 0; // Root directory

    while (token) {
        parent_inode = ifs_find_inode(token);
        if (parent_inode == -1) return -1;
        token = strtok_custom(NULL, "/");
    }

    return parent_inode;
}
