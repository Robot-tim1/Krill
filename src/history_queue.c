#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "history_queue.h"

queue_t *create_queue(int cap) {
    queue_t *queue = malloc(sizeof(queue_t));

    if (!queue) {
        return NULL;
    }

    queue->buffer = malloc(cap * sizeof(char *));

    if (!queue->buffer) {
        free(queue);
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

    bool was_incremented = false;
    queue->head++;
    if (queue->size != queue->capacity) {
        queue->size++;
        was_incremented = true;
    }

    char *temp = malloc(strlen(value) + 1);

    if (!temp) {
        queue->head--;
        if (was_incremented) {
            queue->size--;
        }
        return 0;
    }

    if (queue->buffer + queue->capacity == queue->head) {
        queue->head = queue->buffer;
    }

    if (!was_incremented) {
        free(*queue->head);
    }
    *queue->head = temp;

    strcpy(*queue->head, value);

    return 1;
}

int append_latest(queue_t *queue, char *value) {
    if (value[0] == '\0') {
        return 1;
    }
    
    char *temp = realloc(*queue->head, strlen(*queue->head) + strlen(value) + 1);

    if (!temp) {
        return 0;
    }

    *queue->head = temp;
    strcat(*queue->head, value);
    return 1;
}

char *queue_get(queue_t *queue, int index) { 
    int head_index = (int)(queue->head - queue->buffer);

    if (head_index - index >= 0) {
        return *(queue->head - index);
    }

    index = (-index % queue->capacity) + queue->capacity;
    return *(queue->head + index);
}