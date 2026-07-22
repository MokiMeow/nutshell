#ifdef KERNEL_TEST

#include <stdbool.h>
#include <stdint.h>

#include "heap.h"
#include "io.h"
#include "kprintf.h"
#include "pmm.h"
#include "selftest.h"
#include "shell.h"

#define DEBUG_EXIT_PORT 0xF4
#define TEST_SUCCESS 0
#define TEST_FAILURE 1

static void test_exit(uint32_t status) {
    outl(DEBUG_EXIT_PORT, status);
    for (;;)
        __asm__ volatile ("hlt");
}

void kernel_self_test(void) {
    bool passed = true;

    if (!pmm_self_test()) {
        kprintf("[FAIL] test-pmm\n");
        passed = false;
    } else {
        kprintf("[ok] test-pmm\n");
    }

    if (!heap_self_test()) {
        kprintf("[FAIL] test-heap\n");
        passed = false;
    } else {
        kprintf("[ok] test-heap\n");
    }

    if (!shell_execute("echo scripted-shell-ok")) {
        kprintf("[FAIL] test-shell\n");
        passed = false;
    } else {
        kprintf("[ok] test-shell\n");
    }

    if (passed) {
        kprintf("[ok] kernel-selftest\n");
        test_exit(TEST_SUCCESS);
    }

    kprintf("[FAIL] kernel-selftest\n");
    test_exit(TEST_FAILURE);
}

#endif
