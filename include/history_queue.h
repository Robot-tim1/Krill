#pragma once

typedef struct Queue {
    char **buffer;
    char **head;
    int size;
    int capacity;
} queue_t;

queue_t *create_queue(int cap);
int enqueue(queue_t *queue, char *value);
int append_latest(queue_t *queue, char *value);
char *queue_get(queue_t *queue, int index);