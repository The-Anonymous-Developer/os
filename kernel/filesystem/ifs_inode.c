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