#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct memory_stats {
    uint64_t total_frames;
    uint64_t used_frames;
    uint64_t free_frames;
    size_t heap_used;
    size_t heap_capacity;
};

bool heap_init(void);
void *kmalloc(size_t size);
void kfree(void *pointer);
struct memory_stats mem_stats(void);
bool heap_self_test(void);
