#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "kprintf.h"
#include "serial.h"
#include "string.h"
#include "vga.h"

#define NUMBER_BUFFER_SIZE 65

static void kputc(char character) {
    vga_putc(character);
    if (character == '\n')
        serial_putc('\r');
    serial_putc(character);
}

static int kputs(const char *string) {
    int count = 0;

    if (string == NULL)
        string = "(null)";

    while (*string != '\0') {
        kputc(*string++);
        count++;
    }

    return count;
}

int kprintf(const char *format, ...) {
    char buffer[NUMBER_BUFFER_SIZE];
    int count = 0;
    va_list arguments;

    va_start(arguments, format);

    while (*format != '\0') {
        if (*format != '%') {
            kputc(*format++);
            count++;
            continue;
        }

        format++;
        switch (*format) {
        case 'c':
            kputc((char)va_arg(arguments, int));
            count++;
            break;
        case 's':
            count += kputs(va_arg(arguments, const char *));
            break;
        case 'd':
            count += kputs(itoa(va_arg(arguments, int), buffer, 10));
            break;
        case 'u':
            count += kputs(utoa(va_arg(arguments, unsigned int), buffer, 10));
            break;
        case 'x':
            count += kputs(utoa(va_arg(arguments, unsigned int), buffer, 16));
            break;
        case 'p':
            count += kputs("0x");
            count += kputs(utoa((uintptr_t)va_arg(arguments, void *), buffer,
                                16));
            break;
        case '%':
            kputc('%');
            count++;
            break;
        case '\0':
            kputc('%');
            count++;
            format--;
            break;
        default:
            kputc('%');
            kputc(*format);
            count += 2;
            break;
        }

        format++;
    }

    va_end(arguments);
    return count;
}
