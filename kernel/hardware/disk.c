#include "disk.h"
#include "../hardware/display.h"
#include <stdbool.h>
#include <stdint.h>
#include "../filesystem/ifs.h"
#include "../lib/string.h"

static disk_t disks[MAX_DISKS];

void disk_init() {
    display_write("[DISK] Initializing disk subsystem...\n");
    
    // Simulating available disks (Replace with actual hardware detection)
    for (int i = 0; i < MAX_DISKS; i++) {
        disks[i].id = i;
        disks[i].size = 500 * 1024 * 1024; // Simulating 500MB disks
        disks[i].sector_count = disks[i].size / SECTOR_SIZE;
        disks[i].is_available = true;
    }
}

int disk_count() {
    int count = 0;
    for (int i = 0; i < MAX_DISKS; i++) {
        if (disks[i].is_available) count++;
    }
    return count;
}

disk_t* disk_get(int id) {
    if (id < 0 || id >= MAX_DISKS) return NULL;
    return &disks[id];
}

bool disk_read(int disk_id, uint64_t sector, void* buffer, uint32_t count) {
    if (disk_id < 0 || disk_id >= MAX_DISKS || !disks[disk_id].is_available) {
        return false;
    }

    // TODO: Implement actual hardware sector reading
    display_write("[DISK] Simulated read: Disk ");
    display_write_number(disk_id);
    display_write(", Sector ");
    display_write_number(sector);
    display_write("\n");

    return true;
}

bool disk_write(int disk_id, uint64_t sector, const void* buffer, uint32_t count) {
    if (disk_id < 0 || disk_id >= MAX_DISKS || !disks[disk_id].is_available) {
        return false;
    }

    // TODO: Implement actual hardware sector writing
    display_write("[DISK] Simulated write: Disk ");
    display_write_number(disk_id);
    display_write(", Sector ");
    display_write_number(sector);
    display_write("\n");

    return true;
}
