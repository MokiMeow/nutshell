#pragma once

#include <stddef.h>
#include <stdint.h>

void *memset(void *destination, int value, size_t count);
void *memcpy(void *destination, const void *source, size_t count);
size_t strlen(const char *string);
int strcmp(const char *left, const char *right);
char *utoa(uint64_t value, char *buffer, unsigned int base);
char *itoa(int64_t value, char *buffer, unsigned int base);
