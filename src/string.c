#include <stddef.h>
#include <stdint.h>

#include "string.h"

void *memset(void *destination, int value, size_t count) {
    unsigned char *bytes = destination;

    for (size_t i = 0; i < count; i++)
        bytes[i] = (unsigned char)value;

    return destination;
}

void *memcpy(void *destination, const void *source, size_t count) {
    unsigned char *destination_bytes = destination;
    const unsigned char *source_bytes = source;

    for (size_t i = 0; i < count; i++)
        destination_bytes[i] = source_bytes[i];

    return destination;
}

size_t strlen(const char *string) {
    size_t length = 0;

    while (string[length] != '\0')
        length++;

    return length;
}

char *utoa(uint64_t value, char *buffer, unsigned int base) {
    static const char digits[] = "0123456789abcdef";
    size_t length = 0;

    if (base < 2 || base > 16) {
        buffer[0] = '\0';
        return buffer;
    }

    do {
        buffer[length++] = digits[value % base];
        value /= base;
    } while (value != 0);

    buffer[length] = '\0';

    for (size_t i = 0; i < length / 2; i++) {
        char temporary = buffer[i];
        buffer[i] = buffer[length - i - 1];
        buffer[length - i - 1] = temporary;
    }

    return buffer;
}

char *itoa(int64_t value, char *buffer, unsigned int base) {
    if (base == 10 && value < 0) {
        uint64_t magnitude = (uint64_t)(-(value + 1)) + 1;

        buffer[0] = '-';
        utoa(magnitude, buffer + 1, base);
        return buffer;
    }

    return utoa((uint64_t)value, buffer, base);
}
