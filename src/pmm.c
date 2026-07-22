#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "pmm.h"
#include "string.h"

#define MULTIBOOT_TAG_END 0
#define MULTIBOOT_TAG_MEMORY_MAP 6
#define MULTIBOOT_MEMORY_AVAILABLE 1
#define MULTIBOOT_TAG_ALIGNMENT 8
#define IDENTITY_MAP_LIMIT (1ULL << 30)
#define MAX_FRAME_COUNT (IDENTITY_MAP_LIMIT / PMM_PAGE_SIZE)
#define BITMAP_WORD_BITS 64
#define BITMAP_WORD_COUNT (MAX_FRAME_COUNT / BITMAP_WORD_BITS)

struct multiboot_header {
    uint32_t total_size;
    uint32_t reserved;
};

struct multiboot_tag {
    uint32_t type;
    uint32_t size;
};

struct multiboot_memory_map_tag {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
};

struct multiboot_memory_map_entry {
    uint64_t address;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
} __attribute__((packed));

extern uint8_t end[];

static uint64_t frame_bitmap[BITMAP_WORD_COUNT];
static uint64_t frame_count;
static uint64_t used_frame_count;

static uintptr_t align_up(uintptr_t value, uintptr_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

static bool frame_is_used(uint64_t frame) {
    return (frame_bitmap[frame / BITMAP_WORD_BITS]
            & (1ULL << (frame % BITMAP_WORD_BITS))) != 0;
}

static void mark_frame_used(uint64_t frame) {
    if (frame < frame_count && !frame_is_used(frame)) {
        frame_bitmap[frame / BITMAP_WORD_BITS]
            |= 1ULL << (frame % BITMAP_WORD_BITS);
        used_frame_count++;
    }
}

static void mark_frame_free(uint64_t frame) {
    if (frame < frame_count && frame_is_used(frame)) {
        frame_bitmap[frame / BITMAP_WORD_BITS]
            &= ~(1ULL << (frame % BITMAP_WORD_BITS));
        used_frame_count--;
    }
}

static void reserve_range(uintptr_t start, uintptr_t finish) {
    uint64_t first = start / PMM_PAGE_SIZE;
    uint64_t last = align_up(finish, PMM_PAGE_SIZE) / PMM_PAGE_SIZE;

    if (last > frame_count)
        last = frame_count;
    for (uint64_t frame = first; frame < last; frame++)
        mark_frame_used(frame);
}

static struct multiboot_memory_map_tag *find_memory_map(
        uintptr_t multiboot_address, uintptr_t multiboot_end) {
    uintptr_t cursor = multiboot_address + sizeof(struct multiboot_header);

    while (cursor + sizeof(struct multiboot_tag) <= multiboot_end) {
        struct multiboot_tag *tag = (struct multiboot_tag *)cursor;

        if (tag->type == MULTIBOOT_TAG_END)
            break;
        if (tag->size < sizeof(struct multiboot_tag)
                || cursor + tag->size > multiboot_end)
            return NULL;
        if (tag->type == MULTIBOOT_TAG_MEMORY_MAP)
            return (struct multiboot_memory_map_tag *)tag;

        cursor = align_up(cursor + tag->size, MULTIBOOT_TAG_ALIGNMENT);
    }

    return NULL;
}

static uint64_t clipped_entry_end(
        const struct multiboot_memory_map_entry *entry) {
    uint64_t maximum_length;

    if (entry->address >= IDENTITY_MAP_LIMIT)
        return 0;
    maximum_length = IDENTITY_MAP_LIMIT - entry->address;
    if (entry->length > maximum_length)
        return IDENTITY_MAP_LIMIT;
    return entry->address + entry->length;
}

static bool configure_frames(struct multiboot_memory_map_tag *memory_map) {
    uintptr_t entries_start = (uintptr_t)memory_map + sizeof(*memory_map);
    uintptr_t entries_end = (uintptr_t)memory_map + memory_map->size;
    uint64_t highest_address = 0;

    if (memory_map->size < sizeof(*memory_map)
            || memory_map->entry_size < sizeof(struct multiboot_memory_map_entry))
        return false;

    for (uintptr_t cursor = entries_start;
            cursor + memory_map->entry_size <= entries_end;
            cursor += memory_map->entry_size) {
        struct multiboot_memory_map_entry *entry =
            (struct multiboot_memory_map_entry *)cursor;
        uint64_t entry_end = clipped_entry_end(entry);

        if (entry_end > highest_address)
            highest_address = entry_end;
    }

    frame_count = highest_address / PMM_PAGE_SIZE;
    if (frame_count == 0 || frame_count > MAX_FRAME_COUNT)
        return false;

    memset(frame_bitmap, 0xFF, sizeof(frame_bitmap));
    used_frame_count = frame_count;

    for (uintptr_t cursor = entries_start;
            cursor + memory_map->entry_size <= entries_end;
            cursor += memory_map->entry_size) {
        struct multiboot_memory_map_entry *entry =
            (struct multiboot_memory_map_entry *)cursor;
        uint64_t entry_end = clipped_entry_end(entry);
        uint64_t first;
        uint64_t last;

        if (entry->type != MULTIBOOT_MEMORY_AVAILABLE || entry_end == 0)
            continue;
        first = align_up(entry->address, PMM_PAGE_SIZE) / PMM_PAGE_SIZE;
        last = entry_end / PMM_PAGE_SIZE;
        for (uint64_t frame = first; frame < last; frame++)
            mark_frame_free(frame);
    }

    return true;
}

bool pmm_init(uintptr_t multiboot_info_address) {
    struct multiboot_header *header =
        (struct multiboot_header *)multiboot_info_address;
    uintptr_t multiboot_end;
    struct multiboot_memory_map_tag *memory_map;

    if (multiboot_info_address == 0
            || multiboot_info_address >= IDENTITY_MAP_LIMIT
            || header->total_size < sizeof(*header)
            || header->total_size > IDENTITY_MAP_LIMIT - multiboot_info_address)
        return false;

    multiboot_end = multiboot_info_address + header->total_size;
    memory_map = find_memory_map(multiboot_info_address, multiboot_end);
    if (memory_map == NULL || !configure_frames(memory_map))
        return false;

    reserve_range(0, (uintptr_t)end);
    reserve_range(multiboot_info_address, multiboot_end);
    return used_frame_count < frame_count;
}

uintptr_t pmm_alloc(void) {
    for (uint64_t frame = 0; frame < frame_count; frame++) {
        if (!frame_is_used(frame)) {
            mark_frame_used(frame);
            return frame * PMM_PAGE_SIZE;
        }
    }

    return 0;
}

void pmm_free(uintptr_t frame) {
    if (frame % PMM_PAGE_SIZE == 0)
        mark_frame_free(frame / PMM_PAGE_SIZE);
}

struct pmm_stats pmm_get_stats(void) {
    struct pmm_stats stats = {
        .total_frames = frame_count,
        .used_frames = used_frame_count,
        .free_frames = frame_count - used_frame_count,
    };

    return stats;
}

bool pmm_self_test(void) {
    uintptr_t first = pmm_alloc();
    uintptr_t second = pmm_alloc();
    uintptr_t third = pmm_alloc();
    uintptr_t recycled;
    bool valid = first != 0 && second != 0 && third != 0
        && first != second && first != third && second != third;

    if (!valid) {
        if (first != 0)
            pmm_free(first);
        if (second != 0)
            pmm_free(second);
        if (third != 0)
            pmm_free(third);
        return false;
    }

    pmm_free(second);
    recycled = pmm_alloc();
    valid = recycled == second;

    pmm_free(first);
    pmm_free(third);
    if (recycled != 0)
        pmm_free(recycled);
    return valid;
}
