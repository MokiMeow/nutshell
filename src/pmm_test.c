/* Synthetic boot maps exercise the real PMM without touching allocated RAM.
 * Run before heap initialization; the caller must restore the real boot map. */
#ifdef KERNEL_TEST

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "pmm.h"
#include "string.h"

#define TEST_BASE (16ULL * 1024 * 1024)
#define TEST_LIMIT (1ULL << 30)
#define CHECK(condition) do { if (!(condition)) return __LINE__; } while (0)

static uint64_t fixture[64];
static size_t fixture_size;

static uint32_t *tag_at(size_t offset) {
    return (uint32_t *)((uint8_t *)fixture + offset);
}

static void begin_map(void) {
    memset(fixture, 0, sizeof(fixture));
    fixture_size = 8;
}

static size_t add_tag(uint32_t type, uint32_t size) {
    size_t offset = fixture_size;
    uint32_t *tag = tag_at(offset);

    tag[0] = type;
    tag[1] = size;
    fixture_size += (size + 7) & ~7U;
    return offset;
}

static size_t add_memory_map(uint32_t entries, uint32_t stride) {
    size_t offset = add_tag(6, 16 + entries * stride);
    tag_at(offset)[2] = stride;
    return offset;
}

static void set_entry(size_t map, size_t index, uint64_t address,
                      uint64_t length, uint32_t type) {
    uint32_t stride = tag_at(map)[2];
    uint64_t *entry = (uint64_t *)((uint8_t *)fixture + map + 16
                                 + index * stride);
    entry[0] = address;
    entry[1] = length;
    ((uint32_t *)entry)[4] = type;
}

static size_t add_module(uint32_t start, uint32_t finish) {
    size_t offset = add_tag(3, 17);
    tag_at(offset)[2] = start;
    tag_at(offset)[3] = finish;
    return offset;
}

static void finish_map(void) {
    add_tag(0, 8);
    tag_at(0)[0] = fixture_size;
}

static size_t simple_map(void) {
    size_t map;

    begin_map();
    map = add_memory_map(1, 24);
    set_entry(map, 0, TEST_BASE, 4 * PMM_PAGE_SIZE, 1);
    finish_map();
    return map;
}

static bool initialize(void) {
    return pmm_init((uintptr_t)fixture);
}

static int test_available_ranges(void) {
    size_t map = simple_map();
    uintptr_t frame;

    CHECK(initialize());
    CHECK(pmm_get_stats().free_frames == 4);
    frame = pmm_alloc_contiguous(4);
    CHECK(frame == TEST_BASE);
    CHECK(pmm_alloc() == 0);
    CHECK(pmm_alloc_contiguous(0) == 0);
    CHECK(pmm_alloc_contiguous((size_t)-1) == 0);
    pmm_free(frame + 1);
    pmm_free(0);
    pmm_free(TEST_LIMIT);
    CHECK(pmm_get_stats().free_frames == 0);
    for (size_t i = 0; i < 4; i++)
        pmm_free(frame + i * PMM_PAGE_SIZE);
    pmm_free(frame);
    CHECK(pmm_get_stats().free_frames == 4);

    set_entry(map, 0, TEST_BASE + 1, 4 * PMM_PAGE_SIZE - 2, 1);
    CHECK(initialize());
    CHECK(pmm_get_stats().free_frames == 2);
    CHECK(pmm_alloc_contiguous(2) == TEST_BASE + PMM_PAGE_SIZE);

    set_entry(map, 0, TEST_LIMIT - PMM_PAGE_SIZE, UINT64_MAX, 1);
    CHECK(initialize());
    CHECK(pmm_get_stats().free_frames == 1);
    CHECK(pmm_alloc() == TEST_LIMIT - PMM_PAGE_SIZE);
    CHECK(pmm_alloc() == 0);
    set_entry(map, 0, TEST_LIMIT, PMM_PAGE_SIZE, 1);
    CHECK(!initialize());
    CHECK(pmm_alloc() == 0);
    return 0;
}

static int test_reserved_ranges(void) {
    for (size_t order = 0; order < 2; order++) {
        size_t map;

        begin_map();
        map = add_memory_map(2, 24);
        set_entry(map, order, TEST_BASE, 4 * PMM_PAGE_SIZE, 1);
        set_entry(map, 1 - order, TEST_BASE + PMM_PAGE_SIZE - 1, 2, 2);
        finish_map();
        CHECK(initialize());
        CHECK(pmm_get_stats().free_frames == 2);
        CHECK(pmm_alloc_contiguous(3) == 0);
        CHECK(pmm_alloc_contiguous(2) == TEST_BASE + 2 * PMM_PAGE_SIZE);

        set_entry(map, 1 - order, TEST_BASE + PMM_PAGE_SIZE, 1, 99);
        CHECK(initialize());
        CHECK(pmm_get_stats().free_frames == 3);
        CHECK(pmm_alloc_contiguous(3) == 0);
        CHECK(pmm_alloc() == TEST_BASE);
        CHECK(pmm_alloc_contiguous(2) == TEST_BASE + 2 * PMM_PAGE_SIZE);

        set_entry(map, 1 - order, TEST_BASE + 1, 0, 2);
        CHECK(initialize());
        CHECK(pmm_get_stats().free_frames == 4);
    }
    return 0;
}

static int test_modules(void) {
    for (size_t order = 0; order < 2; order++) {
        size_t map, module;

        begin_map();
        if (order == 0)
            module = add_module(TEST_BASE + PMM_PAGE_SIZE - 1,
                                TEST_BASE + PMM_PAGE_SIZE + 1);
        map = add_memory_map(1, 24);
        set_entry(map, 0, TEST_BASE, 4 * PMM_PAGE_SIZE, 1);
        if (order == 1)
            module = add_module(TEST_BASE + PMM_PAGE_SIZE - 1,
                                TEST_BASE + PMM_PAGE_SIZE + 1);
        finish_map();
        CHECK(initialize());
        CHECK(pmm_get_stats().free_frames == 2);
        CHECK(pmm_alloc() == TEST_BASE + 2 * PMM_PAGE_SIZE);
        pmm_free(TEST_BASE);
        CHECK(pmm_get_stats().free_frames == 1);

        tag_at(module)[3] = tag_at(module)[2];
        CHECK(initialize());
        CHECK(pmm_get_stats().free_frames == 4);
        tag_at(module)[3]--;
        CHECK(!initialize());
        CHECK(pmm_alloc() == 0);
        tag_at(module)[3] = tag_at(module)[2];
        ((uint8_t *)fixture)[module + 16] = 'x';
        CHECK(!initialize()); /* The NUL in tag padding must not count. */
        ((uint8_t *)fixture)[module + 16] = '\0';
        tag_at(module)[1] = 15;
        CHECK(!initialize());
    }

    begin_map();
    size_t map = add_memory_map(1, 24);
    set_entry(map, 0, TEST_BASE, 4 * PMM_PAGE_SIZE, 1);
    add_module(TEST_BASE, TEST_BASE + PMM_PAGE_SIZE + 1);
    add_module(TEST_BASE + PMM_PAGE_SIZE, TEST_BASE + 3 * PMM_PAGE_SIZE);
    finish_map();
    CHECK(initialize());
    CHECK(pmm_get_stats().free_frames == 1);
    CHECK(pmm_alloc() == TEST_BASE + 3 * PMM_PAGE_SIZE);
    return 0;
}

static int test_tag_validation(void) {
    size_t map = simple_map();

    CHECK(!pmm_init(0));
    CHECK(!pmm_init(TEST_LIMIT));
    CHECK(!pmm_init(TEST_LIMIT - 4));
    CHECK(!pmm_init((uintptr_t)fixture + 1));
    CHECK(initialize());
    tag_at(0)[0] = 8;
    CHECK(!initialize());
    CHECK(pmm_get_stats().total_frames == 0);
    CHECK(pmm_alloc() == 0);

    simple_map();
    tag_at(0)[0] = UINT32_MAX;
    CHECK(!initialize());
    simple_map();
    tag_at(0)[1] = 1;
    CHECK(initialize()); /* Reserved boot-header fields must be ignored. */
    simple_map();
    tag_at(fixture_size - 8)[0] = 99;
    CHECK(!initialize());
    simple_map();
    tag_at(fixture_size - 8)[1] = 0;
    CHECK(!initialize());
    simple_map();
    tag_at(fixture_size - 8)[1] = 16;
    CHECK(!initialize());
    simple_map();
    tag_at(0)[0] -= 1;
    CHECK(!initialize());

    for (uint32_t size = 0; size < 8; size++) {
        simple_map();
        tag_at(map)[1] = size;
        CHECK(!initialize());
    }
    simple_map();
    tag_at(map)[1] = UINT32_MAX;
    CHECK(!initialize());
    simple_map();
    tag_at(map)[1] = 8;
    CHECK(!initialize());
    simple_map();
    tag_at(map)[2] = 0;
    CHECK(!initialize());
    simple_map();
    tag_at(map)[2] = 16;
    CHECK(!initialize());
    simple_map();
    tag_at(map)[2] = 25;
    CHECK(!initialize());
    simple_map();
    tag_at(map)[1] = 39;
    CHECK(!initialize());

    simple_map();
    tag_at(map)[0] = 0;
    tag_at(map)[1] = 8;
    CHECK(!initialize()); /* End tags cannot hide trailing boot data. */

    begin_map();
    map = add_memory_map(1, 24);
    set_entry(map, 0, TEST_BASE, 4 * PMM_PAGE_SIZE, 1);
    size_t duplicate = add_memory_map(1, 24);
    set_entry(duplicate, 0, TEST_BASE, 4 * PMM_PAGE_SIZE, 1);
    finish_map();
    CHECK(!initialize());

    begin_map();
    map = add_memory_map(1, 24);
    set_entry(map, 0, TEST_BASE, 4 * PMM_PAGE_SIZE, 1);
    size_t trailing = add_tag(99, 8);
    finish_map();
    tag_at(trailing)[1] = 7;
    CHECK(!initialize()); /* Validate tags after the otherwise valid mmap. */

    map = simple_map();
    set_entry(map, 0, TEST_BASE, 4 * PMM_PAGE_SIZE, 2);
    CHECK(!initialize());
    CHECK(pmm_get_stats().total_frames == 0);
    CHECK(pmm_get_stats().used_frames == 0);
    CHECK(pmm_alloc() == 0);

    begin_map();
    map = add_memory_map(1, 24);
    set_entry(map, 0, TEST_BASE, 4 * PMM_PAGE_SIZE, 1);
    add_module(TEST_BASE, TEST_BASE + 4 * PMM_PAGE_SIZE);
    finish_map();
    CHECK(!initialize());
    CHECK(pmm_get_stats().total_frames == 0);
    CHECK(pmm_alloc() == 0);

    begin_map();
    add_tag(99, 9); /* Unknown tags and their padding remain supported. */
    map = add_memory_map(1, 32);
    tag_at(map)[3] = 1; /* Future versions retain the common entry prefix. */
    set_entry(map, 0, TEST_BASE, 4 * PMM_PAGE_SIZE, 1);
    finish_map();
    CHECK(initialize());
    CHECK(pmm_get_stats().free_frames == 4);

    begin_map();
    map = add_memory_map(3, 24);
    set_entry(map, 0, TEST_BASE, 4 * PMM_PAGE_SIZE, 1);
    set_entry(map, 1, TEST_BASE, 4 * PMM_PAGE_SIZE, 1);
    set_entry(map, 2, TEST_LIMIT - 1, 0, 2);
    finish_map();
    CHECK(initialize());
    CHECK(pmm_get_stats().free_frames == 4);
    CHECK(pmm_get_stats().total_frames == TEST_BASE / PMM_PAGE_SIZE + 4);
    return 0;
}

#ifdef PMM_HOST_TEST
static int test_tag_mutations(void) {
    simple_map();
    uint8_t *bytes = (uint8_t *)fixture;

    /* Keep the physical envelope fixed while mutating every tag byte. The
     * host sanitizer build checks all parser reads on these malformed maps. */
    for (size_t offset = 8; offset < fixture_size; offset++) {
        uint8_t original = bytes[offset];

        for (unsigned int value = 0; value < 256; value++) {
            bytes[offset] = (uint8_t)value;
            if (initialize()) {
                struct pmm_stats stats = pmm_get_stats();
                uintptr_t frame = pmm_alloc();

                CHECK(stats.used_frames + stats.free_frames == stats.total_frames);
                CHECK(stats.total_frames <= TEST_LIMIT / PMM_PAGE_SIZE);
                CHECK(frame != 0 && frame < TEST_LIMIT);
                CHECK(frame % PMM_PAGE_SIZE == 0);
            } else {
                CHECK(pmm_get_stats().total_frames == 0);
                CHECK(pmm_alloc() == 0);
            }
        }
        bytes[offset] = original;
    }
    return 0;
}
#endif

int pmm_map_self_test(void) {
    int result = test_available_ranges();

    if (result == 0)
        result = test_reserved_ranges();
    if (result == 0)
        result = test_modules();
    if (result == 0)
        result = test_tag_validation();
#ifdef PMM_HOST_TEST
    if (result == 0)
        result = test_tag_mutations();
#endif
    return result;
}

#ifdef PMM_HOST_TEST
int main(void) {
    return pmm_map_self_test() == 0 ? 0 : 1;
}
#endif

#endif
