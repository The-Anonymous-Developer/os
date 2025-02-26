#ifndef DISK_H
#define DISK_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_DISKS 4 // Can be expanded later
#define SECTOR_SIZE 512
#define IFS_SUPERBLOCK_OFFSET 0  // Superblock is stored at sector 0

// Disk structure
typedef struct {
    uint8_t id;
    uint64_t size;  // Total size in bytes
    uint32_t sector_count;
    bool is_available;
} disk_t;

// Disk detection & management
void disk_init();
int disk_count();
disk_t* disk_get(int id);

// Disk I/O functions
bool disk_read(int disk_id, uint64_t sector, void* buffer, uint32_t count);
bool disk_write(int disk_id, uint64_t sector, const void* buffer, uint32_t count);

#endif // DISK_H
