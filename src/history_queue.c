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

void enqueue(queue_t *queue, char *value) {
    if (queue->size == 0) {
        *queue->head = value;
        queue->size++;
        return;
    }

    queue->head++;
    if (queue->size != queue->capacity) {
        queue->size++;
    }

    if (queue->buffer + queue->capacity == queue->head) {
        queue->head = queue->buffer;
    }

    *queue->head = value;

    if (queue->tail == queue->head) {
        queue->tail++;
    }

    if (queue->buffer + queue->capacity == queue->tail) {
        queue->tail = queue->buffer;
    }
}

char *queue_get(queue_t *queue, int index) {
    if (!queue->size || index >= queue->size) {
        return NULL;
    }

    if (queue->head - index >= queue->buffer) {
        return *(queue->head - index);
    }

    index = (-index % queue->size) + queue->size;
    return *(queue->head + index);
}