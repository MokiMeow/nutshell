#pragma once

/* Minimal COM1 serial driver. Used for logging and for the CI smoke test,
 * which boots the kernel headless and checks the serial log for the banner. */

void serial_init(void);
void serial_putc(char c);
void serial_puts(const char *s);
