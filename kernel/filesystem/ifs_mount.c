#include "ifs.h"
#include "../hardware/disk.h"
#include "../memory/memory.h"
#include "../hardware/display.h"
#include "../lib/string.h" // Required for memcmp

// Define IFS_MAGIC properly
static const char IFS_MAGIC[IFS_MAGIC_LENGTH] = "INTELLISYSFILES";

static ifs_superblock_t superblock;
static int ifs_mounted = 0;

int ifs_mount() {
    display_write("[IFS] Mounting filesystem...\n");

    // Step 1: Detect Storage Devices
    int disk_id = detect_ifs_disk();
    if (disk_id < 0) {
        display_write("[IFS] No IFS-formatted partition found.\n");
        return -1;
    }

    // Step 2: Read Superblock
    if (!disk_read(disk_id, IFS_SUPERBLOCK_OFFSET, &superblock, sizeof(ifs_superblock_t))) {
        display_write("[IFS] Failed to read superblock.\n");
        return -1;
    }

    // Step 3: Verify Magic Number (Use memcmp for string comparison)
    if (memcmp(superblock.magic, IFS_MAGIC, IFS_MAGIC_LENGTH) != 0) {
        display_write("[IFS] Invalid IFS magic number. Possible corruption or non-IFS partition.\n");
        return -1;
    }

    // Step 4: Load Metadata
    display_write("[IFS] Superblock verified. Loading filesystem metadata...\n");

    // Step 5: Check Journaling & Integrity
    if (superblock.journal_state != IFS_JOURNAL_CLEAN) {
        display_write("[IFS] Journal is dirty. Running recovery...\n");
        ifs_journal_recovery();
    }

    // Step 6: Set Up Root Directory
    if (!load_root_directory()) {
        display_write("[IFS] Failed to load root directory.\n");
        return -1;
    }

    // Mark filesystem as mounted
    ifs_mounted = 1;
    display_write("[IFS] Filesystem mounted successfully!\n");
    return 0;
}

/* --------------------------------------------------------------------------------------------------------------------------------------------- */
// TO BE IMPLEMENTED AS OF 26/02/2025!!!!!!

void* load_file(const char* path) {
    display_write("[IFS] load_file() not implemented yet!\n");
    return NULL;
}

size_t get_file_size(const char* path) {
    display_write("[IFS] get_file_size() not implemented yet!\n");
    return 0; // Placeholder
}

int detect_ifs_disk() {
    display_write("[IFS] detect_ifs_disk() not implemented yet!\n");
    return -1; // No IFS disk found
}

void ifs_journal_recovery() {
    display_write("[IFS] ifs_journal_recovery() not implemented yet!\n");
}

int load_root_directory() {
    display_write("[IFS] load_root_directory() not implemented yet!\n");
    return 0; // Simulating failure for now
}

