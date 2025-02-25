#ifndef RESOURCE_OPT_H
#define RESOURCE_OPT_H

#include <stdint.h>
#include <stdbool.h>

// Optimization thresholds
#define OPT_HIGH_USAGE_THRESHOLD 75
#define OPT_LOW_USAGE_THRESHOLD 25
#define OPT_CONTENTION_THRESHOLD 3
#define ANALYSIS_WINDOW 50  // Ticks to analyze

// Optimization suggestion types
typedef enum {
    OPT_NONE,
    OPT_INCREASE_INSTANCES,
    OPT_DECREASE_INSTANCES,
    OPT_ADJUST_TIMEOUT,
    OPT_REBALANCE_LOAD
} optimization_type_t;

// Optimization suggestion
typedef struct {
    uint32_t resource_id;
    optimization_type_t type;
    uint32_t confidence;    // 0-100%
    char description[128];
} optimization_suggestion_t;

void resource_opt_init(void);
void resource_opt_analyze(void);
optimization_suggestion_t* resource_opt_get_suggestions(uint32_t* count);

#endif // RESOURCE_OPT_H