#pragma once

#include <stdbool.h>

typedef struct Queue queue_t;

typedef struct ReplState {
    queue_t *history;
    char *current_line;
    char *saved_line;
    char *yanked_line;
    char *prompt;
    int line_position;
    int line_length;
    int line_bufsize;
    int history_position;
    bool secondary;
    bool add_newline;
    bool add_space;
} repl_state_t;

char *krill_read_line(repl_state_t *state);

void handle_key_input(repl_state_t *state, char c);
void handle_backspace(repl_state_t *state);
char *handle_enter(repl_state_t *state);

void handle_up_arrow(repl_state_t *state);
void handle_down_arrow(repl_state_t *state);
void handle_left_arrow(repl_state_t *state);
void handle_right_arrow(repl_state_t *state);

void handle_ctrl_y(repl_state_t *state);
void handle_ctrl_k(repl_state_t *state);
void handle_ctrl_u(repl_state_t *state);

void redraw_line(repl_state_t *state);
void reposition_cursor(repl_state_t *state);
void resize_line_buffer(repl_state_t *state);