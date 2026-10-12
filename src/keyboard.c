#include <stdint.h>

#include "io.h"
#include "isr.h"
#include "keyboard.h"
#include "keymap.h"
#include "pic.h"
#include "ringbuf.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_IRQ 1

static struct ringbuf input_buffer;
static struct keymap_state decoder_state;

static void keyboard_interrupt(struct interrupt_frame *frame) {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    char character;

    (void)frame;
    character = keymap_decode(&decoder_state, scancode);
    if (character != '\0')
        ringbuf_push(&input_buffer, character);
}

void keyboard_init(void) {
    ringbuf_init(&input_buffer);
    decoder_state = (struct keymap_state){0};
    irq_register_handler(KEYBOARD_IRQ, keyboard_interrupt);
    pic_unmask(KEYBOARD_IRQ);
}

char keyboard_getchar(void) {
    char character;

    for (;;) {
        __asm__ volatile ("cli" : : : "memory");
        if (ringbuf_pop(&input_buffer, &character)) {
            __asm__ volatile ("sti" : : : "memory");
            return character;
        }
        __asm__ volatile ("sti; hlt" : : : "memory");
    }
}
