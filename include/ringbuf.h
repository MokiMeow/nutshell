#pragma once

#include <stdbool.h>
#include <stddef.h>

#define RINGBUF_CAPACITY 128

struct ringbuf {
    char data[RINGBUF_CAPACITY];
    volatile size_t head;
    volatile size_t tail;
};

void ringbuf_init(struct ringbuf *buffer);
bool ringbuf_push(struct ringbuf *buffer, char value);
bool ringbuf_pop(struct ringbuf *buffer, char *value);
bool ringbuf_empty(const struct ringbuf *buffer);
