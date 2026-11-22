#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

    queue->head = queue->buffer;
    queue->capacity = cap;
    queue->size = 0;

    return queue;
}

int enqueue(queue_t *queue, char *value) {
    if (queue->size == 0) {
        *queue->head = malloc(strlen(value) + 1);
        if (!*queue->head) {
            return 0;
        }

        strcpy(*queue->head, value);
        queue->size++;
        return 1;
    }

    queue->head++;

    if (queue->buffer + queue->capacity == queue->head) {
        queue->head = queue->buffer;
    }

    *queue->head = malloc(strlen(value) + 1);

    if (!*queue->head) {
        return 0;
    }

    strcpy(*queue->head, value);
    if (queue->size != queue->capacity) {
        queue->size++;
    }

    return 1;
}

char *queue_get(queue_t *queue, int index) { 
    if (queue->head - index >= queue->buffer) {
        return *(queue->head - index);
    }

    index = (-index % queue->size) + queue->size;
    return *(queue->head + index);
}