#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_BENCHMARKS 32
#define BENCH_NAME_MAX 32
#define SAMPLES_PER_BENCH 100

typedef struct {
    char name[BENCH_NAME_MAX];
    uint32_t total_time;
    uint32_t min_time;
    uint32_t max_time;
    uint32_t sample_count;
    uint32_t samples[SAMPLES_PER_BENCH];
    uint32_t start_time;    // Added start_time field
    bool is_active;
} benchmark_t;

void benchmark_init(void);
uint32_t benchmark_start(const char* name);
void benchmark_end(uint32_t id);
void benchmark_print_results(void);
void benchmark_reset(uint32_t id);

#endif // BENCHMARK_H