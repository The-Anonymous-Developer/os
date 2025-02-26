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

// Functions
int ifs_mount();
void ifs_journal_recovery();
int load_root_directory();
int detect_ifs_disk();

void* load_file(const char* path);  // Function to read a file into memory
size_t get_file_size(const char* path);  // Function to get file size



#endif // IFS_H
