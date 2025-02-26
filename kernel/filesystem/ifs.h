#ifndef IFS_H
#define IFS_H

#include "../lib/string.h"
#include "../memory/heap.h"
#include <stdint.h>

#define IFS_MAGIC_LENGTH 16  // 16 bytes for "INTELLISYSFILES"
#define IFS_SUPERBLOCK_OFFSET 0
#define IFS_INODE_TABLE_OFFSET 1024
#define MAX_INODES 1024
#define MAX_BLOCKS 12  // Direct block pointers
#define IFS_DISK 0 // Fix for undefined reference
#define BLOCK_SIZE 4096  // 4KB block size

// Journaling states
#define IFS_JOURNAL_CLEAN 0
#define IFS_JOURNAL_DIRTY 1

// File types
#define FILE_TYPE_REGULAR 1
#define FILE_TYPE_DIRECTORY 2

// For file open/close mechanism
#define MAX_OPEN_FILES 128


#define IFS_MAX_FILENAME_LENGTH 255
#define IFS_MAX_DIRECTORY_ENTRIES 128  // Max entries in one directory

// Permission flags
#define IFS_PERM_READ    0x4  // Read permission (r--)
#define IFS_PERM_WRITE   0x2  // Write permission (-w-)
#define IFS_PERM_EXECUTE 0x1  // Execute permission (--x)

// User, Group, Others (3-bit sets)
#define IFS_PERM_USER_SHIFT 6  // User permission shift
#define IFS_PERM_GROUP_SHIFT 3 // Group permission shift
#define IFS_PERM_OTHER_SHIFT 0 // Other permission shift

// Extract permissions
#define IFS_PERM_USER(perm) ((perm >> IFS_PERM_USER_SHIFT) & 0x7)
#define IFS_PERM_GROUP(perm) ((perm >> IFS_PERM_GROUP_SHIFT) & 0x7)
#define IFS_PERM_OTHER(perm) ((perm >> IFS_PERM_OTHER_SHIFT) & 0x7)

typedef struct {
    uint64_t inode_id;                 // Inode ID of the file/directory
    char name[IFS_MAX_FILENAME_LENGTH]; // Filename
} ifs_dir_entry_t;

typedef struct {
    uint32_t fd;
    uint64_t inode_id;
    uint64_t position;
    uint64_t flags;
} ifs_open_file_t;

extern ifs_open_file_t open_files[MAX_OPEN_FILES];

// Superblock structure
typedef struct {
    char magic[IFS_MAGIC_LENGTH];  // Magic number stored as a char array
    uint64_t total_size;  // Total filesystem size
    uint32_t block_size;  // Block size
    uint32_t inode_count; // Number of inodes
    uint32_t journal_state; // Journaling state (clean/dirty)
} ifs_superblock_t;

extern ifs_superblock_t superblock;

// Inode structure
typedef struct {
    uint64_t inode_id;                 // Inode identifier
    char name[256];                    // File or directory name
    uint32_t type;                     // Directory or file
    uint32_t permissions;              // Read, write, execute flags
    uint64_t size;                      // File size (bytes)
    uint64_t created_time;             // Time of creation
    uint64_t modified_time;            // Time of last modification
    uint64_t accessed_time;            // Last accessed timestamp
    uint32_t hard_links;               // Number of links pointing to this inode
    uint32_t uid;                      // Owner of file (e.g. user/system) ID
    uint32_t gid;                      // Group ID
    uint32_t block_count;              // Number of blocks used
    uint64_t block_pointers[MAX_BLOCKS]; // ✅ Fix: Direct data block pointers
    uint64_t indirect_block;           // Points to single indirect data block
    uint64_t double_indirect_block;    // Points to double indirect data block
    uint64_t triple_indirect_block;    // Points to triple indirect data block
    uint32_t flags;                    // Special flags (e.g. sys, hidden files, etc.)
    uint32_t reserved[5];              // Reserved for future use
} ifs_inode_t;

// 🔹 Filesystem functions
int ifs_mount();
void ifs_journal_recovery();
int load_root_directory();
int detect_ifs_disk();

// 🔹 File operations
void* load_file(const char* path);
size_t get_file_size(const char* path);
int ifs_create_file(const char *path, uint16_t mode);
int ifs_delete_file(const char *path);
int ifs_open(const char *path, uint32_t flags);
int ifs_read_file(const char *path, void *buffer, size_t size);
int ifs_write_file(const char *path, const void *buffer, size_t size);

// 🔹 Inode management functions
void ifs_load_inode_table();
int ifs_allocate_inode();
ifs_inode_t* ifs_get_inode(uint64_t inode_id);
void ifs_update_inode(uint64_t inode_id);
int ifs_delete_inode(uint64_t inode_id);

// Directory management functions
int ifs_create_directory(const char *path);
int ifs_delete_directory(const char *path);
int ifs_list_directory(const char *path);
int ifs_resolve_path(const char *path);


// 🔹 Block management functions
uint64_t allocate_block(); 
void free_block(uint64_t block_id); 

// 🔹 Permission & Ownership Functions
int ifs_check_permissions(ifs_inode_t *inode, uint32_t requested_perm);
int ifs_chmod(const char *path, uint32_t new_permissions);
int ifs_chown(const char *path, uint32_t new_uid, uint32_t new_gid);


#endif // IFS_H
