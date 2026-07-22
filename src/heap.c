#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "heap.h"
#include "pmm.h"

#define HEAP_FRAME_COUNT 16
#define HEAP_ALIGNMENT 16

static uint8_t *heap_base;
static size_t heap_offset;
static size_t heap_capacity;

static size_t align_size(size_t size) {
    return (size + HEAP_ALIGNMENT - 1) & ~(HEAP_ALIGNMENT - 1);
}

bool heap_init(void) {
    uintptr_t first_frame = 0;

    for (size_t i = 0; i < HEAP_FRAME_COUNT; i++) {
        uintptr_t frame = pmm_alloc();

        if (frame == 0 || (i > 0 && frame != first_frame + i * PMM_PAGE_SIZE)) {
            if (frame != 0)
                pmm_free(frame);
            for (size_t j = 0; j < i; j++)
                pmm_free(first_frame + j * PMM_PAGE_SIZE);
            return false;
        }
        if (i == 0)
            first_frame = frame;
    }

    heap_base = (uint8_t *)first_frame;
    heap_offset = 0;
    heap_capacity = HEAP_FRAME_COUNT * PMM_PAGE_SIZE;
    return true;
}

void *kmalloc(size_t size) {
    size_t allocation_size;
    void *allocation;

    if (size == 0)
        return NULL;
    allocation_size = align_size(size);
    if (allocation_size > heap_capacity - heap_offset)
        return NULL;

    allocation = heap_base + heap_offset;
    heap_offset += allocation_size;
    return allocation;
}

void kfree(void *pointer) {
    (void)pointer;
}

struct memory_stats mem_stats(void) {
    struct pmm_stats physical = pmm_get_stats();
    struct memory_stats stats = {
        .total_frames = physical.total_frames,
        .used_frames = physical.used_frames,
        .free_frames = physical.free_frames,
        .heap_used = heap_offset,
        .heap_capacity = heap_capacity,
    };

    return stats;
}

bool heap_self_test(void) {
    void *first = kmalloc(31);
    void *second = kmalloc(65);

    return first != NULL && second != NULL && first != second
        && (uintptr_t)first % HEAP_ALIGNMENT == 0
        && (uintptr_t)second % HEAP_ALIGNMENT == 0;
}
