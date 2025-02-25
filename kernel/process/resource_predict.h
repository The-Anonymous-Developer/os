#ifndef RESOURCE_PREDICT_H
#define RESOURCE_PREDICT_H

#include <stdint.h>

#define PREDICTION_WINDOW 10    // Number of future samples to predict
#define TREND_HISTORY 20       // Number of samples to use for trend analysis

typedef struct {
    int32_t slope;            // Rate of change
    uint32_t predicted_values[PREDICTION_WINDOW];
    uint32_t confidence;      // Prediction confidence (0-100)
} resource_prediction_t;

void resource_predict_init(void);
void resource_predict_update(uint32_t resource_id);
resource_prediction_t* resource_predict_get(uint32_t resource_id);
void resource_predict_print(uint32_t resource_id);

#endif // RESOURCE_PREDICT_H