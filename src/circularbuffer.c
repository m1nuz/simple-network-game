#include "circularbuffer.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Circular buffer object */
struct circular_buffer
{
    size_t      capacity;       // maximum number of elements
    size_t      start;          // index of oldest element
    size_t      end;            // index at which to write new element
    size_t      element_size;
    void        *elements;      // vector of elements
};

extern CIRCULAR_BUFFER *
new_circular_buffer(size_t capacity, size_t element_size)
{
    assert(capacity > 1);
    assert(element_size > 1);

    CIRCULAR_BUFFER *cb = malloc(sizeof(CIRCULAR_BUFFER));
    cb->elements = calloc(capacity + 1, element_size);

    if(cb->elements == NULL)
    {
        fprintf(stderr, "circular buffer: %s\n", "can\'t alloc memory");
        return NULL;
    }

    cb->capacity = capacity + 1; // include empty element
    cb->start = 0;
    cb->end   = 0;
    cb->element_size = element_size;

    return cb;
}

extern void
delete_circular_buffer(CIRCULAR_BUFFER *cb)
{
    assert(cb != NULL);

    free(cb->elements);
    free(cb);
}

extern void
write_circular_buffer(CIRCULAR_BUFFER *cb, const void *element)
{
    assert(cb != NULL);

    memcpy(cb->elements + cb->end * cb->element_size, element, cb->element_size);

    cb->end = (cb->end + 1) % cb->capacity;

    if (cb->end == cb->start)
        cb->start = (cb->start + 1) % cb->capacity;
}

extern void
read_circular_buffer(CIRCULAR_BUFFER *cb, void *element)
{
    assert(cb != NULL);
    assert(element != NULL);

    memcpy(element, cb->elements + cb->start * cb->element_size, cb->element_size);

    cb->start = (cb->start + 1) % cb->capacity;
}

extern void
clear_circular_buffer(CIRCULAR_BUFFER *cb)
{
    assert(cb != NULL);
    cb->start = 0;
    cb->end   = 0;
}

extern int
is_full_circular_buffer(CIRCULAR_BUFFER *cb)
{
    assert(cb != NULL);

    return (cb->end + 1) % cb->capacity == cb->start;
}

extern int
is_empty_circular_buffer(CIRCULAR_BUFFER *cb)
{
    assert(cb != NULL);

    return cb->end == cb->start;
}

// example
/*typedef struct { int value; } ElemType;

int main(int argc, char **argv) {
    CIRCULAR_BUFFER *cb;
    ElemType elem = {0};

    int testBufferSize = 10; // arbitrary size
    cb = new_circular_buffer(testBufferSize, sizeof(ElemType));

    // Fill buffer with test elements 3 times
    for (elem.value = 0; elem.value < 3 * testBufferSize; ++ elem.value)
        write_circular_buffer(cb, &elem);

    // Remove and print all elements
    while (!is_empty_circular_buffer(cb)) {
        read_circular_buffer(cb, &elem);
        printf("%d\n", elem.value);
    }

    delete_circular_buffer(cb);

    return 0;
}*/
