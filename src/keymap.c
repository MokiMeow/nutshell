#include <stdbool.h>
#include <stdint.h>

#include "keymap.h"

#define SCANCODE_RELEASE_BIT 0x80
#define SCANCODE_EXTENDED 0xE0
#define SCANCODE_LEFT_SHIFT 0x2A
#define SCANCODE_RIGHT_SHIFT 0x36
#define SCANCODE_CAPS_LOCK 0x3A

static const char keymap[128] = {
    [0x02] = '1', [0x03] = '2', [0x04] = '3', [0x05] = '4',
    [0x06] = '5', [0x07] = '6', [0x08] = '7', [0x09] = '8',
    [0x0A] = '9', [0x0B] = '0', [0x0C] = '-', [0x0D] = '=',
    [0x0E] = '\b', [0x0F] = '\t', [0x10] = 'q', [0x11] = 'w',
    [0x12] = 'e', [0x13] = 'r', [0x14] = 't', [0x15] = 'y',
    [0x16] = 'u', [0x17] = 'i', [0x18] = 'o', [0x19] = 'p',
    [0x1A] = '[', [0x1B] = ']', [0x1C] = '\n', [0x1E] = 'a',
    [0x1F] = 's', [0x20] = 'd', [0x21] = 'f', [0x22] = 'g',
    [0x23] = 'h', [0x24] = 'j', [0x25] = 'k', [0x26] = 'l',
    [0x27] = ';', [0x28] = '\'', [0x29] = '`', [0x2B] = '\\',
    [0x2C] = 'z', [0x2D] = 'x', [0x2E] = 'c', [0x2F] = 'v',
    [0x30] = 'b', [0x31] = 'n', [0x32] = 'm', [0x33] = ',',
    [0x34] = '.', [0x35] = '/', [0x39] = ' ',
};

static const char shifted_keymap[128] = {
    [0x02] = '!', [0x03] = '@', [0x04] = '#', [0x05] = '$',
    [0x06] = '%', [0x07] = '^', [0x08] = '&', [0x09] = '*',
    [0x0A] = '(', [0x0B] = ')', [0x0C] = '_', [0x0D] = '+',
    [0x0E] = '\b', [0x0F] = '\t', [0x10] = 'Q', [0x11] = 'W',
    [0x12] = 'E', [0x13] = 'R', [0x14] = 'T', [0x15] = 'Y',
    [0x16] = 'U', [0x17] = 'I', [0x18] = 'O', [0x19] = 'P',
    [0x1A] = '{', [0x1B] = '}', [0x1C] = '\n', [0x1E] = 'A',
    [0x1F] = 'S', [0x20] = 'D', [0x21] = 'F', [0x22] = 'G',
    [0x23] = 'H', [0x24] = 'J', [0x25] = 'K', [0x26] = 'L',
    [0x27] = ':', [0x28] = '"', [0x29] = '~', [0x2B] = '|',
    [0x2C] = 'Z', [0x2D] = 'X', [0x2E] = 'C', [0x2F] = 'V',
    [0x30] = 'B', [0x31] = 'N', [0x32] = 'M', [0x33] = '<',
    [0x34] = '>', [0x35] = '?', [0x39] = ' ',
};

/* Keep scancode decoding pure so modifier behavior can be tested on a host
 * without privileged port I/O. Extended scancodes are not yet interpreted. */
char keymap_decode(struct keymap_state *state, uint8_t scancode) {
    bool released;
    uint8_t key;
    bool shifted;
    char character;

    if (scancode == SCANCODE_EXTENDED) {
        state->extended_scancode = true;
        return '\0';
    }
    if (state->extended_scancode) {
        state->extended_scancode = false;
        return '\0';
    }

    released = (scancode & SCANCODE_RELEASE_BIT) != 0;
    key = scancode & (uint8_t)~SCANCODE_RELEASE_BIT;

    if (key == SCANCODE_LEFT_SHIFT) {
        state->left_shift = !released;
        return '\0';
    }
    if (key == SCANCODE_RIGHT_SHIFT) {
        state->right_shift = !released;
        return '\0';
    }
    if (key == SCANCODE_CAPS_LOCK) {
        // A release is not another press: toggle only on the make code.
        if (!released)
            state->caps_lock = !state->caps_lock;
        return '\0';
    }
    if (released)
        return '\0';

    shifted = state->left_shift || state->right_shift;
    character = shifted ? shifted_keymap[key] : keymap[key];
    // Caps Lock affects letters only. Shift still selects symbols and numbers;
    // combined Shift+Caps Lock produces lowercase alphabetic characters.
    if (state->caps_lock && character >= 'a' && character <= 'z')
        character = (char)(character - 'a' + 'A');
    else if (state->caps_lock && character >= 'A' && character <= 'Z')
        character = (char)(character - 'A' + 'a');

    return character;
}
