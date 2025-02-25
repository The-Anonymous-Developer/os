#include "../hardware/hardware.h"
#include "../hardware/timer.h"
#include "../hardware/keyboard.h"
#include "../process/message.h"
#include "../process/deadlock.h"
#include "../process/resource_stats.h"
#include "../process/resource_opt.h"
#include "../process/resource_predict.h"
#include "../process/benchmark.h"

void init_system(void) {
    init_hardware();
    timer_init();
    keyboard_init();
    message_system_init();
    deadlock_init();
    resource_stats_init();
    resource_opt_init();
    resource_predict_init();
    benchmark_init();
}