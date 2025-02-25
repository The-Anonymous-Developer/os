#include "resource_stats.h"
#include "../hardware/display.h"
#include "../lib/string.h"
#include <stddef.h>

// Define the stats array (no longer static)
resource_stats_t stats[MAX_RESOURCES];

void resource_stats_init(void) {
    for (uint32_t i = 0; i < MAX_RESOURCES; i++) {
        stats[i].acquisitions = 0;
        stats[i].acquisition_fails = 0;
        stats[i].wait_time = 0;
        stats[i].hold_time = 0;
        stats[i].deadlocks_prevented = 0;
        stats[i].current_usage = 0;
        stats[i].peak_usage = 0;
        stats[i].alert_triggered = false;
        stats[i].history.head = 0;
        stats[i].history.count = 0;
        stats[i].sample_interval = 100;  // Sample every 100 ticks
        stats[i].last_sample_time = 0;
    }
    display_write("Resource statistics tracking initialized\n");
}

static void update_resource_usage(uint32_t resource_id) {
    if (resource_id >= MAX_RESOURCES) return;

    uint32_t total_attempts = stats[resource_id].acquisitions + 
                            stats[resource_id].acquisition_fails;
    
    if (total_attempts > 0) {
        stats[resource_id].current_usage = 
            (stats[resource_id].acquisition_fails * 100) / total_attempts;
        
        if (stats[resource_id].current_usage > stats[resource_id].peak_usage) {
            stats[resource_id].peak_usage = stats[resource_id].current_usage;
        }
    }
}

void resource_stats_check_alerts(void) {
    char buffer[128];
    
    for (uint32_t i = 0; i < MAX_RESOURCES; i++) {
        update_resource_usage(i);
        
        if (!stats[i].alert_triggered && 
            stats[i].current_usage >= RESOURCE_ALERT_THRESHOLD) {
            
            sprintf_simple(buffer,
                "ALERT: Resource %u high usage (%u%%)\n",
                i, stats[i].current_usage);
            display_write(buffer);
            
            stats[i].alert_triggered = true;
        }
        
        if (stats[i].acquisition_fails >= RESOURCE_HIGH_CONTENTION) {
            sprintf_simple(buffer,
                "WARNING: Resource %u high contention (%u failed attempts)\n",
                i, stats[i].acquisition_fails);
            display_write(buffer);
        }
    }
}

void resource_stats_acquire(uint32_t resource_id, uint32_t wait_time) {
    if (resource_id < MAX_RESOURCES) {
        stats[resource_id].acquisitions++;
        stats[resource_id].wait_time += wait_time;
    }
}

void resource_stats_release(uint32_t resource_id, uint32_t hold_time) {
    if (resource_id < MAX_RESOURCES) {
        stats[resource_id].hold_time += hold_time;
    }
}

void resource_stats_fail(uint32_t resource_id) {
    if (resource_id < MAX_RESOURCES) {
        stats[resource_id].acquisition_fails++;
    }
}

void resource_stats_deadlock_prevented(uint32_t resource_id) {
    if (resource_id < MAX_RESOURCES) {
        stats[resource_id].deadlocks_prevented++;
    }
}

void resource_stats_print(void) {
    char buffer[128];
    display_write("\nResource Statistics:\n");
    
    for (uint32_t i = 0; i < MAX_RESOURCES; i++) {
        if (stats[i].acquisitions > 0 || stats[i].acquisition_fails > 0) {
            sprintf_simple(buffer, 
                "Resource %u:\n"
                "  Acquisitions: %u\n"
                "  Failed attempts: %u\n"
                "  Avg wait time: %u\n"
                "  Avg hold time: %u\n"
                "  Deadlocks prevented: %u\n"
                "  Current usage: %u%%\n"
                "  Peak usage: %u%%\n",
                i,
                stats[i].acquisitions,
                stats[i].acquisition_fails,
                stats[i].acquisitions ? stats[i].wait_time / stats[i].acquisitions : 0,
                stats[i].acquisitions ? stats[i].hold_time / stats[i].acquisitions : 0,
                stats[i].deadlocks_prevented,
                stats[i].current_usage,
                stats[i].peak_usage
            );
            display_write(buffer);
        }
    }
}

void resource_stats_update_history(uint32_t resource_id) {
    if (resource_id >= MAX_RESOURCES) return;
    
    resource_stats_t* stat = &stats[resource_id];
    uint32_t current_time = get_system_ticks();
    
    // Update history every sample interval
    if (current_time - stat->last_sample_time >= stat->sample_interval) {
        usage_history_t* history = &stat->history;
        
        // Add new sample
        history->usage[history->head] = stat->current_usage;
        history->head = (history->head + 1) % HISTORY_SIZE;
        if (history->count < HISTORY_SIZE) {
            history->count++;
        }
        
        stat->last_sample_time = current_time;
    }
}

void resource_stats_draw_graph(uint32_t resource_id) {
    if (resource_id >= MAX_RESOURCES) return;
    
    resource_stats_t* stat = &stats[resource_id];
    usage_history_t* history = &stat->history;
    char buffer[128];
    
    sprintf_simple(buffer, "Resource %u Usage Graph:\n", resource_id);
    display_write(buffer);
    
    // Draw Y axis labels
    display_write("100% |");
    display_write(" 75% |");
    display_write(" 50% |");
    display_write(" 25% |");
    display_write("  0% |");
    
    // Draw graph points
    for (uint32_t i = 0; i < history->count; i++) {
        uint32_t idx = (history->head - 1 - i + HISTORY_SIZE) % HISTORY_SIZE;
        uint32_t usage = history->usage[idx];
        
        // Convert usage to graph height (0-4)
        uint32_t height = (usage * 4) / 100;
        char point = (height > 0) ? '*' : '_';
        
        buffer[i] = point;
    }
    buffer[history->count] = '\0';
    display_write(buffer);
    
    // Draw X axis
    display_write("     +-------------------------------->");
    display_write("     Time -->");
}