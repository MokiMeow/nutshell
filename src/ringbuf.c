#include <stdbool.h>
#include <stddef.h>

#include "ringbuf.h"

/* Wrap using a conditional instead of integer remainder in the keyboard IRQ. */
static size_t next_index(size_t index) {
    index++;
    return index == RINGBUF_CAPACITY ? 0 : index;
}

void ringbuf_init(struct ringbuf *buffer) {
    buffer->head = 0;
    buffer->tail = 0;
}

bool ringbuf_push(struct ringbuf *buffer, char value) {
    size_t next = next_index(buffer->head);

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
    buffer->tail = next_index(buffer->tail);
    return true;
}

bool ringbuf_empty(const struct ringbuf *buffer) {
    return buffer->head == buffer->tail;
}
