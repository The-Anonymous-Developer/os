#include "resource_predict.h"
#include "resource_stats.h"
#include "../hardware/display.h"
#include "../lib/string.h"

static resource_prediction_t predictions[MAX_RESOURCES];

void resource_predict_init(void) {
    for (uint32_t i = 0; i < MAX_RESOURCES; i++) {
        predictions[i].slope = 0;
        predictions[i].confidence = 0;
        for (uint32_t j = 0; j < PREDICTION_WINDOW; j++) {
            predictions[i].predicted_values[j] = 0;
        }
    }
    display_write("Resource prediction initialized\n");
}

static int32_t calculate_trend(usage_history_t* history) {
    if (history->count < TREND_HISTORY) return 0;
    
    int32_t sum_xy = 0, sum_x = 0, sum_y = 0, sum_xx = 0;
    uint32_t n = TREND_HISTORY;
    
    for (uint32_t i = 0; i < n; i++) {
        uint32_t idx = (history->head - 1 - i + HISTORY_SIZE) % HISTORY_SIZE;
        int32_t x = i;
        int32_t y = history->usage[idx];
        
        sum_xy += x * y;
        sum_x += x;
        sum_y += y;
        sum_xx += x * x;
    }
    
    // Calculate slope using linear regression
    return ((n * sum_xy) - (sum_x * sum_y)) / ((n * sum_xx) - (sum_x * sum_x));
}

void resource_predict_update(uint32_t resource_id) {
    if (resource_id >= MAX_RESOURCES) return;
    
    usage_history_t* history = &stats[resource_id].history;
    resource_prediction_t* pred = &predictions[resource_id];
    
    // Calculate trend
    pred->slope = calculate_trend(history);
    
    // Get latest usage as base
    uint32_t latest = history->usage[(history->head - 1 + HISTORY_SIZE) % HISTORY_SIZE];
    
    // Generate predictions
    for (uint32_t i = 0; i < PREDICTION_WINDOW; i++) {
        int32_t predicted = latest + (pred->slope * (i + 1));
        pred->predicted_values[i] = (predicted < 0) ? 0 : 
                                  (predicted > 100) ? 100 : predicted;
    }
    
    // Calculate confidence based on history size
    pred->confidence = (history->count * 100) / HISTORY_SIZE;
}

resource_prediction_t* resource_predict_get(uint32_t resource_id) {
    if (resource_id >= MAX_RESOURCES) return NULL;
    return &predictions[resource_id];
}

void resource_predict_print(uint32_t resource_id) {
    if (resource_id >= MAX_RESOURCES) return;
    
    resource_prediction_t* pred = &predictions[resource_id];
    char buffer[128];
    
    sprintf_simple(buffer, "\nResource %u Usage Prediction (Confidence: %u%%):\n",
                  resource_id, pred->confidence);
    display_write(buffer);
    
    sprintf_simple(buffer, "Trend: %s%d%% per interval\n",
                  (pred->slope >= 0) ? "+" : "", pred->slope);
    display_write(buffer);
    
    display_write("Future usage: ");
    for (uint32_t i = 0; i < PREDICTION_WINDOW; i++) {
        sprintf_simple(buffer, "%u%% ", pred->predicted_values[i]);
        display_write(buffer);
    }
    display_write("\n");
}