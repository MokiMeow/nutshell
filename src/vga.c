/* Minimal VGA text-mode driver.
 *
 * The VGA text buffer lives at physical 0xB8000 and is a grid of 80x25
 * 16-bit cells: low byte = character, high byte = colour attribute. This
 * is available in the legacy BIOS boot path we use, so writing here puts
 * characters on screen with no driver initialisation. */

#include <stdint.h>
#include <stddef.h>
#include "vga.h"

#define VGA_ADDR 0xB8000
#define VGA_COLS 80
#define VGA_ROWS 25

static volatile uint16_t *const vga = (volatile uint16_t *)VGA_ADDR;
static size_t row = 0, col = 0;
static uint8_t color = (VGA_BLACK << 4) | VGA_WHITE;

static inline uint16_t cell(char c, uint8_t attr) {
    return (uint16_t)(uint8_t)c | ((uint16_t)attr << 8);
}

void vga_set_color(uint8_t fg, uint8_t bg) {
    color = (uint8_t)((bg << 4) | (fg & 0x0F));
}

void vga_clear(void) {
    for (size_t i = 0; i < VGA_COLS * VGA_ROWS; i++)
        vga[i] = cell(' ', color);
    row = col = 0;
}

static void scroll(void) {
    for (size_t r = 1; r < VGA_ROWS; r++)
        for (size_t c = 0; c < VGA_COLS; c++)
            vga[(r - 1) * VGA_COLS + c] = vga[r * VGA_COLS + c];
    for (size_t c = 0; c < VGA_COLS; c++)
        vga[(VGA_ROWS - 1) * VGA_COLS + c] = cell(' ', color);
    row = VGA_ROWS - 1;
}

static void newline(void) {
    col = 0;
    if (++row == VGA_ROWS)
        scroll();
}

void vga_putc(char c) {
    if (c == '\n') { newline(); return; }
    vga[row * VGA_COLS + col] = cell(c, color);
    if (++col == VGA_COLS)
        newline();
}

void vga_puts(const char *s) {
    for (; *s; s++)
        vga_putc(*s);
}
