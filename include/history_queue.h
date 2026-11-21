#pragma once

typedef struct Queue {
    char **data;
    char **head;
    char **tail;
    int size;
    int capacity;
} queue_t;

queue_t *create_queue(int size);