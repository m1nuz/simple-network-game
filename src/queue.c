#include "queue.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifdef __GNUC__

#ifndef _cas
#define _cas(ptr, oldval, newval) \
         __sync_bool_compare_and_swap(ptr, oldval, newval)
#endif

#ifndef _inc
#define _inc(ptr) __sync_fetch_and_add(ptr, 1)
#endif

#ifndef _dec
#define _dec(ptr) __sync_fetch_and_add(ptr, -1)
#endif

#endif

struct queue_node
{
    struct queue_node   *next;
    int                *item;
};

struct queue_root
{
    struct queue_node   *head;
    struct queue_node   *tail;

    int                 (*push)(QUEUE *, const void *);
    void *              (*pop)(QUEUE *queue, void *);
    void *              (*peek)(QUEUE *queue);
    void                (*del)(QUEUE *queue);

    size_t              size;
    size_t              item_size;
};
/*
 * Default
 */
void
default_delete_queue(QUEUE *queue)
{
    struct queue_node *head = queue->head;

    int check = queue->size;

    while ((head != NULL) && check)
    {
        struct queue_node *next = head->next;

        free(head);

        check--;
        head = next;
    }
}

int
default_push_queue(QUEUE *queue, const void *item)
{
    assert(queue != NULL);

    struct queue_node *node = malloc(sizeof(struct queue_node) + queue->item_size);

    if (node == NULL)
        return 0;

    //node->item = item;
    node->item = node + sizeof(struct queue_node);
    memcpy(&node->item, &item, queue->item_size);

    node->next = NULL;

    if (queue->head == NULL)
        queue->head = node;
    else
        queue->tail->next = node;

    queue->tail = node;
    queue->size++;

    return 1;
}

void *
default_pop_queue(QUEUE *queue, void *item)
{
    struct queue_node *head = queue->head;

    if((head == NULL) || (item == NULL))
        return NULL;

    //void *item = head->item;
    memcpy(item, head->item, queue->item_size);

    queue->head = head->next;
    queue->size--;

    free(head);

    return item;
}

void *
default_peek_queue(QUEUE *queue)
{
    struct queue_node *head = queue->head;

    if (head == NULL)
        return NULL;

    return head->item;
}

/*
 * Lockfree
 */
int
lockfree_push_queue(QUEUE *queue, const void *item)
{
    struct queue_node *n;
    struct queue_node *node = malloc(sizeof(struct queue_node) + queue->item_size);

    node->next = NULL;

    //node->item = item;
    node->item = node + sizeof(struct queue_node);
    memcpy(&node->item, &item, queue->item_size);

    while (1)
    {
        n = queue->tail;

        if (_cas(&(n->next), NULL, node))
            break;
        else
            _cas(&(queue->tail), n, n->next);
    }

    _cas(&(queue->tail), n, node);

    _inc(&queue->size);

    return 1;
}

void *
lockfree_pop_queue(QUEUE *queue, void *item)
{
    struct queue_node *n;
    //void *item;

    while (1)
    {
        n = queue->head;

        if (n->next == NULL)
            return NULL;

        if(_cas(&(queue->head), n, n->next))
            break;
    }

    //item = (void *) n->next->item;
    memcpy(item, n->next->item, queue->item_size);

    free(n);

    _dec(&queue->size);

    return item;
}

void *
lockfree_peek_queue(QUEUE *queue)
{
    struct queue_node *head = queue->head;

    if (head == NULL)
        return NULL;

    return head->item;
}

void
lockfree_delete_queue(QUEUE *queue, void *item)
{
    while(queue->size > 0)
        lockfree_pop_queue(queue, item);
}

QUEUE*
new_queue(int usage, size_t item_size)
{
    QUEUE *q = malloc(sizeof(QUEUE));

    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    q->item_size = item_size;

    switch(usage)
    {
    case QDEFAULT:
        q->push = default_push_queue;
        q->pop = default_pop_queue;
        q->peek = default_peek_queue;
        q->del = default_delete_queue;
        break;
    case QLOCKFREE:
        q->head = malloc(sizeof(struct queue_node));
        q->head->item = NULL;
        q->head->next = NULL;
        q->tail = q->head;

        q->push = lockfree_push_queue;
        q->pop = lockfree_pop_queue;
        q->peek = lockfree_peek_queue;
        q->del = lockfree_delete_queue;
        break;
    default:
        q->push = NULL;
        q->pop = NULL;
        q->peek = NULL;
        q->del = NULL;
    }

    return q;
}

void
delete_queue(QUEUE *queue)
{
    assert(queue != NULL);

    queue->del(queue);

    free(queue);
    queue = NULL;
}

int
push_queue(QUEUE *queue, const void *item)
{
    assert(queue != NULL);

    return queue->push(queue, item);
}

void *
pop_queue(QUEUE *queue, void *item)
{
    assert(queue != NULL);

    return queue->pop(queue, item);
}

void *
peek_queue(QUEUE *queue)
{
    assert(queue != NULL);

    return queue->peek(queue);
}

int
queue_size(QUEUE *queue)
{
    assert(queue != NULL);

    return queue->size;
}

/*void
test()
{
    int v[4] = {1, 2, 3, 4};
    int s[4] = {};

    QUEUE *q = new_queue(QDEFAULT, sizeof(int));

    for(int i = 0; i < 4; i++)
    {
        push_queue(q, &v[i]);
    }

    for(int i = 0; i < 4; i++)
    {
        int *p = pop_queue(q, &s[i]);
        printf("%d\n", s[i]);
    }

    delete_queue(q);

    exit(0);
}*/

