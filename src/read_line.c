#include <stdio.h>
#include <stdlib.h>

#include "read_line.h"

#define LSH_RL_BUFSIZE 1024

char *lsh_read_line(void) {
    size_t bufsize = 0;
    char *line = NULL;

    if (getline(&line, &bufsize, stdin) == -1){
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);
        } else {
            perror("readline");
            exit(EXIT_FAILURE);
        }
    }

    return line;
}