#ifndef RING_BUFFER_H
#define RING_BUFFER_H
#include <stddef.h>
#include <stdlib.h>

#include "defs.h"

/* Implement the backflip flipping queue as a ring buffer */
typedef struct ring_buffer *ring_buffer_t;
struct ring_buffer
{
    uint16_t *raw_ptr_index;
    uint16_t *start_ptr_index;
    uint16_t *raw_ptr_position;
    uint16_t *start_ptr_position;
    /* Extra information on a position that might be useful */
    int *raw_ptr_extra;
    int *start_ptr_extra;
    size_t size;
    size_t start_idx;
    size_t length;
};

ring_buffer_t rb_alloc(size_t size);
void rb_free(ring_buffer_t rb);
void rb_prepend(ring_buffer_t rb, uint16_t index, uint16_t position, int extra);
void rb_append(ring_buffer_t rb, uint16_t index, uint16_t position, int extra);
void rb_get_first(ring_buffer_t rb, uint16_t *index, uint16_t *position,
                  int *extra);
void rb_get(ring_buffer_t rb, size_t i, uint16_t *index, uint16_t *position,
            int *extra);
void rb_remove_first(ring_buffer_t rb);
void rb_remove(ring_buffer_t rb, size_t i);
void rb_put(ring_buffer_t rb, size_t i, uint16_t index, uint16_t position,
            int extra);
#endif
