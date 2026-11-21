#include <stdlib.h>

#include "history_queue.h"

queue_t *create_queue(int size) {
    queue_t *queue = malloc(sizeof(queue_t));

    if (!queue) {
        return NULL;
    }

    queue->data = malloc(size * sizeof(char *));

    if (!queue->data) {
        return NULL;
    }

    queue->tail = queue->data;
    queue->head = queue->data;
    queue->capacity = size;
    queue->size = 0;

    return queue;
}