#include <stdlib.h>

#include "history_queue.h"

queue_t *create_queue(int cap) {
    queue_t *queue = malloc(sizeof(queue_t));

    if (!queue) {
        return NULL;
    }

    queue->buffer = malloc(cap * sizeof(char *));

    if (!queue->buffer) {
        return NULL;
    }

    queue->tail = queue->buffer;
    queue->head = queue->buffer;
    queue->capacity = cap;
    queue->size = 0;

    return queue;
}