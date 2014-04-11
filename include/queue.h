#pragma once

#include <stddef.h>

#define QDEFAULT    0
#define QLOCKFREE   1

typedef struct queue_root QUEUE;

QUEUE* new_queue(int usage, size_t item_size);
void delete_queue(QUEUE *queue);
int push_queue(QUEUE *queue, const void *item);
void *pop_queue(QUEUE *queue, void *item);
void *peek_queue(QUEUE *queue);
int queue_size(QUEUE *queue);
