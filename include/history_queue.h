#pragma once

typedef struct Queue {
    char **buffer;
    char **head;
    char **tail;
    int size;
    int capacity;
} queue_t;

queue_t *create_queue(int cap);