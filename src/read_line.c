#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "read_line.h"
#include "terminal_mode.h"

#define LSH_RL_BUFSIZE 4096
#define CLEAR_LINE "\x1b[2K\r"

char *lsh_read_line(repl_state_t *state) {
    char *line = calloc(LSH_RL_BUFSIZE, sizeof(char));

    if (!line) {
        fprintf(stderr, "\nlsh: allocation error\n");
        return NULL;
    }
    
    state->current_line = line;
    state->line_position = -1;
    int line_size = 2;

    enable_raw_mode();
    char c;
    char seq[3];

    while (1) {
        while (read(STDIN_FILENO, &c, 1) != 1);

        switch (c)
        {
        case '\033':
            // will add support for multi byte escape sequences another time
            if (read(STDIN_FILENO, &seq[0], 1) != 1) continue;
            if (read(STDIN_FILENO, &seq[1], 1) != 1) continue;

            break;
        case '\x03':
            puts("^C");
            disable_raw_mode();
            free(state->current_line);
            exit(EXIT_SUCCESS);
        case '\n':
        case '\r':
            disable_raw_mode();
            fputs("\n", stdout);
            state->line_position++;
            state->current_line[state->line_position] = '\n';
            state->current_line[state->line_position + 1] = '\0';
            return state->current_line;
        case '\x7f':
        case '\x08':
            if (strlen(state->current_line) > 0 && state->line_position != -1) {
                line_size--;
                state->current_line[state->line_position] = '\0';
                state->line_position--;
                printf("%s%s%s", CLEAR_LINE, state->prompt, state->current_line);
                fflush(stdout);
            }
            break;
        default:
            if (c >= 32 && c <= 126 && line_size < LSH_RL_BUFSIZE) {
                line_size++;
                state->line_position++;
                state->current_line[state->line_position] = c;
                printf("%s%s%s", CLEAR_LINE, state->prompt, state->current_line);
                fflush(stdout);
            }
            break;
        }
    }
}