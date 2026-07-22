#include <stdbool.h>
#include <stddef.h>

#include "ringbuf.h"

void ringbuf_init(struct ringbuf *buffer) {
    buffer->head = 0;
    buffer->tail = 0;
}

bool ringbuf_push(struct ringbuf *buffer, char value) {
    size_t next = (buffer->head + 1) % RINGBUF_CAPACITY;

    if (next == buffer->tail)
        return false;

    buffer->data[buffer->head] = value;
    buffer->head = next;
    return true;
}

bool ringbuf_pop(struct ringbuf *buffer, char *value) {
    if (ringbuf_empty(buffer))
        return false;

    *value = buffer->data[buffer->tail];
    buffer->tail = (buffer->tail + 1) % RINGBUF_CAPACITY;
    return true;
}

bool ringbuf_empty(const struct ringbuf *buffer) {
    return buffer->head == buffer->tail;
}
