#pragma once

typedef struct ReplState {
    char **history;
    char *current_line;
    char *saved_line;
    char *yanked_line;
    char *prompt;
    int line_position;
    int history_position;
} repl_state_t;

char *lsh_read_line(repl_state_t *state);