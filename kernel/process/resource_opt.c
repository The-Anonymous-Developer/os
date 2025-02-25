#include "resource_opt.h"
#include "resource_stats.h"
#include "../hardware/display.h"
#include "../lib/string.h"

#define MAX_SUGGESTIONS 16

static optimization_suggestion_t suggestions[MAX_SUGGESTIONS];
static uint32_t suggestion_count = 0;

void resource_opt_init(void) {
    suggestion_count = 0;
    display_write("Resource optimization initialized\n");
}

static void add_suggestion(uint32_t resource_id, optimization_type_t type, 
                         uint32_t confidence, const char* desc) {
    if (suggestion_count >= MAX_SUGGESTIONS) return;
    
    optimization_suggestion_t* sug = &suggestions[suggestion_count++];
    sug->resource_id = resource_id;
    sug->type = type;
    sug->confidence = confidence;
    strcpy(sug->description, desc);
}

void resource_opt_analyze(void) {
    suggestion_count = 0;
    char desc[128];
    
    for (uint32_t i = 0; i < MAX_RESOURCES; i++) {
        usage_history_t* history = &stats[i].history;
        uint32_t high_usage_count = 0;
        uint32_t low_usage_count = 0;
        
        // Analyze recent history
        for (uint32_t j = 0; j < ANALYSIS_WINDOW && j < history->count; j++) {
            uint32_t idx = (history->head - 1 - j + HISTORY_SIZE) % HISTORY_SIZE;
            uint32_t usage = history->usage[idx];
            
            if (usage >= OPT_HIGH_USAGE_THRESHOLD) high_usage_count++;
            if (usage <= OPT_LOW_USAGE_THRESHOLD) low_usage_count++;
        }
        
        // Generate suggestions based on patterns
        if (high_usage_count > ANALYSIS_WINDOW / 2) {
            sprintf_simple(desc, 
                "Resource %u shows sustained high usage (%u%%). Consider increasing instances.",
                i, stats[i].current_usage);
            add_suggestion(i, OPT_INCREASE_INSTANCES, 80, desc);
        }
        
        if (low_usage_count > ANALYSIS_WINDOW / 2) {
            sprintf_simple(desc,
                "Resource %u shows low utilization (%u%%). Consider decreasing instances.",
                i, stats[i].current_usage);
            add_suggestion(i, OPT_DECREASE_INSTANCES, 70, desc);
        }
        
        if (stats[i].acquisition_fails >= OPT_CONTENTION_THRESHOLD) {
            sprintf_simple(desc,
                "Resource %u shows high contention. Consider rebalancing load.",
                i);
            add_suggestion(i, OPT_REBALANCE_LOAD, 90, desc);
        }
    }
}

optimization_suggestion_t* resource_opt_get_suggestions(uint32_t* count) {
    if (count) *count = suggestion_count;
    return suggestions;
}