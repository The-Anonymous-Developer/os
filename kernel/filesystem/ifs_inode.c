#include "ifs.h"
#include "../memory/memory.h"
#include "../hardware/display.h"
#include "../hardware/disk.h"
#include "../lib/string.h"
#include "../hardware/timer.h"

#define IFS_INODE_TABLE_OFFSET 1024    // assuming inode table starts at 1KB
#define MAX_INODES 1024                // number of inodes inside the filesystem

static ifs_inode_t inode_table[MAX_INODES];


void ifs_load_inode_table(){
    display_write("[IFS] Loading inode table...\n");
    disk_read(0, IFS_INODE_TABLE_OFFSET, inode_table, sizeof(ifs_inode_t) * MAX_INODES);
}

// Allocate inode
int ifs_allocate_inode(){
    for(int i = 0; i < MAX_INODES; i++){
        if(inode_table[i].inode_id == 0){           // free inode
            inode_table[i].inode_id = i + 1;        // assign inode id
            inode_table[i].created_time = get_system_ticks();
            inode_table[i].modified_time = inode_table[i].created_time;
            inode_table[i].accessed_time = inode_table[i].created_time;
            display_write("[IFS] Inode allocated\n");
            return i;
        }
    }
    display_write("[IFS] No free inode found\n");
    return -1;
}

// receive inode via id
ifs_inode_t* ifs_get_inode(uint64_t inode_id){
    if (inode_id == 0 || inode_id > MAX_INODES) return NULL;
    return &inode_table[inode_id - 1];
}

// update inode on disk
void ifs_update_inode(uint64_t inode_id) {
    if(inode_id == 0 || inode_id > MAX_INODES) return;
    disk_write(0, IFS_INODE_TABLE_OFFSET + inode_id * sizeof(ifs_inode_t), &inode_table[inode_id - 1], sizeof(ifs_inode_t));
}

// to delete an inode on disk
int ifs_delete_inode(uint64_t inode_id){
    if(inode_id == 0 || inode_id > MAX_INODES) return -1;
    memset(&inode_table[inode_id - 1], 0, sizeof(ifs_inode_t));
    ifs_update_inode(inode_id);
    display_write("[IFS] Inode deleted\n");
    return 0;
}

void debug_inode_creation() {
    display_write("[IFS DEBUG] Testing inode creation...\n");

    // Allocate an inode
    int inode_id = ifs_allocate_inode();
    if (inode_id == -1) {
        display_write("[IFS DEBUG] ERROR: No free inodes available!\n");
        return;
    }

    // Retrieve inode
    ifs_inode_t* inode = ifs_get_inode(inode_id);
    if (!inode) {
        display_write("[IFS DEBUG] ERROR: Failed to retrieve allocated inode!\n");
        return;
    }

    // Print inode details
    display_write("[IFS DEBUG] Inode allocated successfully:\n");
    display_write("  - Inode ID: ");
    display_write_number(inode->inode_id);
    display_write("\n");
    display_write("  - Created Time: ");
    display_write_number(inode->created_time);
    display_write("\n");
    display_write("  - Modified Time: ");
    display_write_number(inode->modified_time);
    display_write("\n");
    display_write("  - Accessed Time: ");
    display_write_number(inode->accessed_time);
    display_write("\n");

    // Persist inode update
    ifs_update_inode(inode_id);
}

void debug_block_allocation() {
    display_write("[IFS DEBUG] Testing block allocation...\n");

    // Allocate an inode
    int inode_id = ifs_allocate_inode();
    if (inode_id == -1) {
        display_write("[IFS DEBUG] ERROR: No free inodes available!\n");
        return;
    }

    // Retrieve inode
    ifs_inode_t* inode = ifs_get_inode(inode_id);
    if (!inode) {
        display_write("[IFS DEBUG] ERROR: Failed to retrieve allocated inode!\n");
        return;
    }

    // Allocate a block
    uint64_t block = allocate_block();
    if (block == 0) {
        display_write("[IFS DEBUG] ERROR: Failed to allocate block!\n");
        return;
    }

    // Assign the block to inode
    inode->block_pointers[0] = block;
    inode->block_count = 1;
    ifs_update_inode(inode_id);

    // Verify block assignment
    display_write("[IFS DEBUG] Block allocated successfully:\n");
    display_write("  - Inode ID: ");
    display_write_number(inode->inode_id);
    display_write("\n");
    display_write("  - Assigned Block: ");
    display_write_number(inode->block_pointers[0]);
    display_write("\n");
}



