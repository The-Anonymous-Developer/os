#ifndef IFS_H
#define IFS_H

#include "../lib/string.h"
#include "../memory/heap.h"
#include <stdint.h>

#define IFS_MAGIC_LENGTH 16  // 16 bytes for "INTELLISYSFILES"

// Journaling states
#define IFS_JOURNAL_CLEAN 0
#define IFS_JOURNAL_DIRTY 1

// Superblock structure
typedef struct {
    char magic[IFS_MAGIC_LENGTH];  // Magic number stored as a char array
    uint64_t total_size;  // Total filesystem size
    uint32_t block_size;  // Block size
    uint32_t inode_count; // Number of inodes
    uint32_t journal_state; // Journaling state (clean/dirty)
} ifs_superblock_t;

typedef struct {
    uint64_t inode_id;                 //inode identifier
    uint32_t type;                     //directory or file
    uint32_t permissions;              // read, write, execute flags
    uint64_t size;                     // file syze (bytes)
    uint64_t created_time;             // time of creation
    uint64_t modified_time;            // time of last modification
    uint64_t accessed_time;            // last accessed timestamp
    uint32_t hard_links;               // number of points linking to this inode
    uint32_t uid;                      // owner of file (eg. user/system) id
    uint32_t gid;                      // group id
    uint32_t block_count;              // number of blocks used
    uint64_t direct_blocks[12];        // direct data block pointers
    uint64_t indirect_block;           // point to single indirect data block
    uint64_t double_indirect_block;   // point to double indirect data block
    uint64_t triple_indirect_block;   // point to triple indirect data block
    uint32_t flags;                     // special flags (eg. sys, hidden files, etc.)
    uint32_t reserved[5];               // reserved for future use
} ifs_inode_t;

// Functions
int ifs_mount();
void ifs_journal_recovery();
int load_root_directory();
int detect_ifs_disk();

void* load_file(const char* path);  // Function to read a file into memory
size_t get_file_size(const char* path);  // Function to get file size



#endif // IFS_H
