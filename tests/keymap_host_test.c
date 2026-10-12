/* Host-executable tests for the freestanding PS/2 set-1 decoder.
 * No QEMU, ports, libc, or third-party test framework is required. */
#include <stdbool.h>
#include <stdint.h>

#include "keymap.h"

#define CHECK(condition) do { if (!(condition)) return __LINE__; } while (0)

int main(void) {
    struct keymap_state state = {0};

    CHECK(keymap_decode(&state, 0x1E) == 'a');
    CHECK(keymap_decode(&state, 0x9E) == '\0'); /* A released */
    CHECK(keymap_decode(&state, 0x02) == '1');
    CHECK(keymap_decode(&state, 0x2A) == '\0'); /* left shift pressed */
    CHECK(keymap_decode(&state, 0x1E) == 'A');
    CHECK(keymap_decode(&state, 0x02) == '!');
    CHECK(keymap_decode(&state, 0xAA) == '\0'); /* left shift released */

    CHECK(keymap_decode(&state, 0x3A) == '\0'); /* caps lock pressed */
    CHECK(state.caps_lock);
    CHECK(keymap_decode(&state, 0xBA) == '\0'); /* release: no toggle */
    CHECK(state.caps_lock);
    CHECK(keymap_decode(&state, 0x1E) == 'A');
    CHECK(keymap_decode(&state, 0x02) == '1');
    CHECK(keymap_decode(&state, 0x36) == '\0'); /* right shift pressed */
    CHECK(keymap_decode(&state, 0x1E) == 'a');
    CHECK(keymap_decode(&state, 0x02) == '!');
    CHECK(keymap_decode(&state, 0xB6) == '\0');

    CHECK(keymap_decode(&state, 0xE0) == '\0');
    CHECK(keymap_decode(&state, 0x48) == '\0'); /* extended up arrow */
    CHECK(keymap_decode(&state, 0x1C) == '\n'); /* enter */
    CHECK(keymap_decode(&state, 0x0E) == '\b'); /* backspace */
    CHECK(keymap_decode(&state, 0x39) == ' ');  /* space */

    CHECK(keymap_decode(&state, 0x3A) == '\0');
    CHECK(!state.caps_lock);
    CHECK(keymap_decode(&state, 0x1E) == 'a');
    return 0;
}
