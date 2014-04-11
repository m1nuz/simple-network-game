#pragma once

#include <stddef.h>

typedef struct circular_buffer CIRCULAR_BUFFER;

CIRCULAR_BUFFER * new_circular_buffer(size_t capacity, size_t element_size);
void delete_circular_buffer(CIRCULAR_BUFFER *cb);
void write_circular_buffer(CIRCULAR_BUFFER *cb, const void *element);
void read_circular_buffer(CIRCULAR_BUFFER *cb, void *element);
void clear_circular_buffer(CIRCULAR_BUFFER *cb);
int is_full_circular_buffer(CIRCULAR_BUFFER *cb);
int is_empty_circular_buffer(CIRCULAR_BUFFER *cb);
