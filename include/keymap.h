#pragma once

#include <stdbool.h>
#include <stdint.h>

/* Per-keyboard modifier state for PS/2 scancode set 1. */
struct keymap_state {
    bool left_shift;
    bool right_shift;
    bool caps_lock;
    bool extended_scancode;
};

/* Translate a scancode into an ASCII character, or zero for a modifier,
 * release, or unsupported extended key event. No hardware I/O is performed. */
char keymap_decode(struct keymap_state *state, uint8_t scancode);
