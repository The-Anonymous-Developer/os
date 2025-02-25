#ifndef RESOURCE_STATS_H
#define RESOURCE_STATS_H
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "resource_defs.h"

#define MAX_RESOURCES 64
#define RESOURCE_ALERT_THRESHOLD 80  // 80% usage triggers alert
#define RESOURCE_HIGH_CONTENTION 5   // 5+ failed attempts indicates contention
#define HISTORY_SIZE 60  // Keep last 60 samples

// Resource usage history
typedef struct {
    uint32_t usage[HISTORY_SIZE];
    uint32_t head;
    uint32_t count;
} usage_history_t;

// Statistics for each resource
typedef struct {
    uint32_t acquisitions;       // Total number of successful acquisitions
    uint32_t acquisition_fails;  // Number of failed acquisition attempts
    uint32_t wait_time;         // Total time tasks spent waiting
    uint32_t hold_time;         // Total time resources were held
    uint32_t deadlocks_prevented; // Number of prevented deadlocks
    uint32_t current_usage;      // Current usage percentage
    uint32_t peak_usage;        // Peak usage percentage
    bool alert_triggered;        // Has this resource triggered an alert?
    usage_history_t history;    // Historical usage data
    uint32_t sample_interval;   // Sampling interval in ticks
    uint32_t last_sample_time;  // Last time usage was sampled
} resource_stats_t;

// Make stats array accessible to other modules
extern resource_stats_t stats[MAX_RESOURCES];

void resource_stats_init(void);
void resource_stats_acquire(uint32_t resource_id, uint32_t wait_time);
void resource_stats_release(uint32_t resource_id, uint32_t hold_time);
void resource_stats_fail(uint32_t resource_id);
void resource_stats_deadlock_prevented(uint32_t resource_id);
void resource_stats_print(void);
void resource_stats_check_alerts(void);
void resource_stats_update_history(uint32_t resource_id);
void resource_stats_draw_graph(uint32_t resource_id);

#endif // RESOURCE_STATS_H