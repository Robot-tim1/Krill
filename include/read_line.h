#pragma once

typedef struct Queue queue_t;

typedef struct ReplState {
    queue_t *history;
    char *current_line;
    char *saved_line;
    char *yanked_line;
    char *prompt;
    int line_position;
    int history_position;
    int line_length;
} repl_state_t;

char *lsh_read_line(repl_state_t *state);
void handle_key_input(repl_state_t *state, char c);
void handle_backspace(repl_state_t *state);
char *handle_enter(repl_state_t *state);
void handle_up_arrow(repl_state_t *state);
void handle_down_arrow(repl_state_t *state);
void handle_left_arrow(repl_state_t *state);
void handle_right_arrow(repl_state_t *state);
void redraw_line(repl_state_t *state);
void reposition_cursor(repl_state_t *state);