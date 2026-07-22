#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "heap.h"
#include "pmm.h"
#include "string.h"

#define HEAP_FRAME_COUNT 16
#define HEAP_ALIGNMENT 16
#define HEAP_SELF_TEST_ALLOCATION_COUNT 12

struct heap_block {
    size_t size;
    struct heap_block *previous;
    struct heap_block *next;
    bool free;
};

static uint8_t *heap_base;
static size_t heap_capacity;
static size_t heap_used;
static struct heap_block *first_block;

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
    heap_capacity = HEAP_FRAME_COUNT * PMM_PAGE_SIZE;
    heap_used = 0;
    first_block = (struct heap_block *)heap_base;
    first_block->size = heap_capacity - sizeof(*first_block);
    first_block->previous = NULL;
    first_block->next = NULL;
    first_block->free = true;
    return true;
}

static void split_block(struct heap_block *block, size_t requested_size) {
    struct heap_block *remainder;

    if (block->size < requested_size + sizeof(*block) + HEAP_ALIGNMENT)
        return;

    remainder = (struct heap_block *)((uint8_t *)(block + 1)
                                      + requested_size);
    remainder->size = block->size - requested_size - sizeof(*block);
    remainder->previous = block;
    remainder->next = block->next;
    remainder->free = true;
    if (remainder->next != NULL)
        remainder->next->previous = remainder;

    block->size = requested_size;
    block->next = remainder;
}

void *kmalloc(size_t size) {
    size_t requested_size;
    struct heap_block *block;

    if (size == 0 || size > heap_capacity)
        return NULL;
    requested_size = align_size(size);

    for (block = first_block; block != NULL; block = block->next) {
        if (block->free && block->size >= requested_size) {
            split_block(block, requested_size);
            block->free = false;
            heap_used += block->size;
            return block + 1;
        }
    }

    return NULL;
}

static void merge_with_next(struct heap_block *block) {
    struct heap_block *next = block->next;

    if (next == NULL || !next->free)
        return;

    block->size += sizeof(*block) + next->size;
    block->next = next->next;
    if (block->next != NULL)
        block->next->previous = block;
}

static struct heap_block *find_block(void *pointer) {
    for (struct heap_block *block = first_block;
            block != NULL; block = block->next) {
        if ((void *)(block + 1) == pointer)
            return block;
    }

    return NULL;
}

void kfree(void *pointer) {
    struct heap_block *block;

    if (pointer == NULL)
        return;
    block = find_block(pointer);
    if (block == NULL || block->free)
        return;

    block->free = true;
    heap_used -= block->size;
    merge_with_next(block);
    if (block->previous != NULL && block->previous->free) {
        block = block->previous;
        merge_with_next(block);
    }
}

struct memory_stats mem_stats(void) {
    struct pmm_stats physical = pmm_get_stats();
    struct memory_stats stats = {
        .total_frames = physical.total_frames,
        .used_frames = physical.used_frames,
        .free_frames = physical.free_frames,
        .heap_used = heap_used,
        .heap_capacity = heap_capacity,
    };

    return stats;
}

bool heap_self_test(void) {
    static const size_t sizes[HEAP_SELF_TEST_ALLOCATION_COUNT] = {
        24, 63, 96, 127, 160, 191, 224, 255, 288, 319, 352, 383,
    };
    void *allocations[HEAP_SELF_TEST_ALLOCATION_COUNT];
    void *replacements[HEAP_SELF_TEST_ALLOCATION_COUNT / 2];

    for (size_t i = 0; i < HEAP_SELF_TEST_ALLOCATION_COUNT; i++) {
        allocations[i] = kmalloc(sizes[i]);
        if (allocations[i] == NULL
                || (uintptr_t)allocations[i] % HEAP_ALIGNMENT != 0)
            return false;
        memset(allocations[i], (int)(i + 1), sizes[i]);
    }

    for (size_t i = 0; i < HEAP_SELF_TEST_ALLOCATION_COUNT; i += 2)
        kfree(allocations[i]);

    for (size_t i = 1; i < HEAP_SELF_TEST_ALLOCATION_COUNT; i += 2) {
        uint8_t *bytes = allocations[i];

        for (size_t j = 0; j < sizes[i]; j++) {
            if (bytes[j] != (uint8_t)(i + 1))
                return false;
        }
    }

    for (size_t i = 0; i < HEAP_SELF_TEST_ALLOCATION_COUNT / 2; i++) {
        replacements[i] = kmalloc(32 + i * 16);
        if (replacements[i] == NULL)
            return false;
    }

    for (size_t i = 1; i < HEAP_SELF_TEST_ALLOCATION_COUNT; i += 2)
        kfree(allocations[i]);
    for (size_t i = 0; i < HEAP_SELF_TEST_ALLOCATION_COUNT / 2; i++)
        kfree(replacements[i]);

    return heap_used == 0 && first_block->free
        && first_block->previous == NULL && first_block->next == NULL
        && first_block->size == heap_capacity - sizeof(*first_block);
}
