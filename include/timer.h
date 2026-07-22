#pragma once

#include <stdint.h>

#define TIMER_FREQUENCY_HZ 100

void timer_init(void);
uint64_t timer_ticks(void);
