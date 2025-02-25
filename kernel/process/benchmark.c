#include "benchmark.h"
#include "../hardware/timer.h"
#include "../hardware/display.h"
#include "../lib/string.h"

static benchmark_t benchmarks[MAX_BENCHMARKS];
static uint32_t benchmark_count = 0;

void benchmark_init(void) {
    benchmark_count = 0;
    for (uint32_t i = 0; i < MAX_BENCHMARKS; i++) {
        benchmarks[i].is_active = false;
    }
    display_write("Benchmark system initialized\n");
}

uint32_t benchmark_start(const char* name) {
    if (benchmark_count >= MAX_BENCHMARKS) return -1;

    benchmark_t* bench = &benchmarks[benchmark_count];
    strcpy(bench->name, name);
    bench->total_time = 0;
    bench->min_time = UINT32_MAX;
    bench->max_time = 0;
    bench->sample_count = 0;
    bench->is_active = true;
    bench->start_time = get_timer_ticks();  // Record start time

    return benchmark_count++;
}

void benchmark_end(uint32_t id) {
    if (id >= MAX_BENCHMARKS || !benchmarks[id].is_active) return;

    benchmark_t* bench = &benchmarks[id];
    uint32_t end_time = get_timer_ticks();
    uint32_t duration = end_time - bench->start_time;

    bench->samples[bench->sample_count++] = duration;
    bench->total_time += duration;
    
    if (duration < bench->min_time) bench->min_time = duration;
    if (duration > bench->max_time) bench->max_time = duration;

    if (bench->sample_count >= SAMPLES_PER_BENCH) {
        benchmark_print_results();
        benchmark_reset(id);
    }
}

void benchmark_print_results(void) {
    char buffer[128];
    display_write("\nBenchmark Results:\n");

    for (uint32_t i = 0; i < benchmark_count; i++) {
        benchmark_t* bench = &benchmarks[i];
        if (!bench->is_active || bench->sample_count == 0) continue;

        uint32_t avg = bench->total_time / bench->sample_count;
        
        sprintf_simple(buffer, 
            "%s:\n"
            "  Samples: %u\n"
            "  Avg Time: %u ticks\n"
            "  Min Time: %u ticks\n"
            "  Max Time: %u ticks\n",
            bench->name,
            bench->sample_count,
            avg,
            bench->min_time,
            bench->max_time
        );
        display_write(buffer);
    }
}

void benchmark_reset(uint32_t id) {
    if (id >= MAX_BENCHMARKS) return;
    benchmarks[id].sample_count = 0;
    benchmarks[id].total_time = 0;
    benchmarks[id].min_time = UINT32_MAX;
    benchmarks[id].max_time = 0;
}