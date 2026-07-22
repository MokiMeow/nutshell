#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PMM_PAGE_SIZE 4096

struct pmm_stats {
    uint64_t total_frames;
    uint64_t used_frames;
    uint64_t free_frames;
};

bool pmm_init(uintptr_t multiboot_info_address);
uintptr_t pmm_alloc(void);
uintptr_t pmm_alloc_contiguous(size_t count);
void pmm_free(uintptr_t frame);
struct pmm_stats pmm_get_stats(void);
bool pmm_self_test(void);
