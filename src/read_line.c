#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "read_line.h"
#include "terminal_mode.h"
#include "history_queue.h"

#define RL_BUFSIZE 4096
#define CLEAR_LINE "\x1b[2K\r"

char *krill_read_line(repl_state_t *state) {
    char *line = calloc(RL_BUFSIZE, sizeof(char));

    if (!line) {
        fprintf(stderr, "\nKrill: allocation error\n");
        return NULL;
    }

    char *saved_line = malloc(RL_BUFSIZE);

    if (!saved_line) {
        fprintf(stderr, "\nKrill: allocation error\n");
        free(line);
        return NULL;
    }
    
    state->current_line = line;
    state->line_position = -1;
    state->history_position = -1;
    state->saved_line = saved_line;
    state->line_length = 0;

    enable_raw_mode();
    char c;
    char seq[3];

    fputs(state->prompt, stdout);
    fflush(stdout);
    while (1) {
        while (read(STDIN_FILENO, &c, 1) != 1);

        switch (c)
        {
        case '\033':

            if (read(STDIN_FILENO, &seq[0], 1) != 1) continue;
            if (read(STDIN_FILENO, &seq[1], 1) != 1) continue;

            if (seq[0] == '[') {
                switch (seq[1])
                {
                case 'A':
                    handle_up_arrow(state);
                    break;
                case 'B':
                    handle_down_arrow(state);
                    break;
                case 'C':
                    handle_right_arrow(state);    
                    break;
                case 'D':
                    handle_left_arrow(state);
                    break;
                }
            }
            break;
        case '\x03':
            disable_raw_mode();
            puts("^C");
            
            free(state->current_line);
            free(state->saved_line);
            state->saved_line = NULL;
            
            exit(EXIT_SUCCESS);
        case '\f':
            printf("\033[H\033[2J%s%s", state->prompt, state->current_line);
            fflush(stdout);
            break;
        case '\n':
        case '\r':
            disable_raw_mode();
            fputs("\n", stdout); 
            return handle_enter(state);
        case '\x7f':
        case '\x08':
            handle_backspace(state);
            break;
        default:
            handle_key_input(state, c);
            break;
        }
    }
}

void redraw_line(repl_state_t *state) {
    printf("%s%s%s", CLEAR_LINE, state->prompt, state->current_line);
    fflush(stdout);
}

void reposition_cursor(repl_state_t *state) {
    if (state->line_position != state->line_length - 1) {
        printf("\x1b[%dD", state->line_length - state->line_position - 1);
    }
    fflush(stdout);
}

void handle_key_input(repl_state_t *state, char c) {
    if (c >= 32 && c <= 126 && state->line_length + 2 < RL_BUFSIZE) {
        state->line_position++;
        state->line_length++;

        memmove(state->current_line + state->line_position + 1, state->current_line + state->line_position, strlen(state->current_line) - state->line_position + 1);
        state->current_line[state->line_position] = c;
        state->current_line[state->line_length] = '\0';

        redraw_line(state);
        reposition_cursor(state);
    }
}

void handle_backspace(repl_state_t *state) {
    if (state->line_position != -1 && state->current_line[0] != '\0') {
        state->line_length--;

        memmove(state->current_line + state->line_position, state->current_line + state->line_position + 1, strlen(state->current_line) - state->line_position);
        state->line_position--;

        redraw_line(state);
        reposition_cursor(state);
    }
}

char *handle_enter(repl_state_t *state) {
    if (strlen(state->current_line) != 0 && (!state->history->size || strcmp(queue_get(state->history, 0), state->current_line) != 0)) {
        if (enqueue(state->history, state->current_line) == 0) {
            fprintf(stderr, "Krill: error saving command to history");
            return NULL;
        }
    }

    state->line_position = state->line_length;
    state->current_line[state->line_position] = '\n';
    state->current_line[state->line_position + 1] = '\0';
    free(state->saved_line);
    state->saved_line = NULL;
            
    return state->current_line;
}

void handle_up_arrow(repl_state_t *state) {
    if (state->history->size && state->history_position < state->history->size - 1) {
        if (state->history_position == -1) {
            strcpy(state->saved_line, state->current_line);
        }

        state->history_position++;

        strcpy(state->current_line, queue_get(state->history, state->history_position));
        state->line_length = strlen(state->current_line);
        state->line_position = state->line_length - 1;

        redraw_line(state);
    }
}

void handle_down_arrow(repl_state_t *state) {
    if (state->history_position > 0) {
        state->history_position--;
        strcpy(state->current_line, queue_get(state->history, state->history_position));

        state->line_length = strlen(state->current_line);
        state->line_position = state->line_length - 1;
        redraw_line(state);
    } else if (state->history_position == 0) {
        state->history_position--;
        strcpy(state->current_line, state->saved_line);

        state->line_length = strlen(state->current_line);
        state->line_position = state->line_length - 1;
        redraw_line(state); 
    }
}

void handle_right_arrow(repl_state_t *state) {
    if (state->current_line[state->line_position+1] != '\0') {
        state->line_position++;
        fputs("\x1b[C", stdout);
        fflush(stdout);
    }
}

void handle_left_arrow(repl_state_t *state) {
    if (state->line_position != -1) {
        state->line_position--;
        fputs("\x1b[D", stdout);
        fflush(stdout);
    }
}