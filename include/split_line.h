#pragma once

typedef struct TokenState {
    char *token;
    char **tokens;
    char ***commands;
    int word_position;
    int token_position;
    int command_position;
    int is_double_quoted;
    int is_single_quoted;
    int token_bufsize;
    int tokens_bufsize;
    int command_bufsize;
} token_state_t;

typedef struct ReplState repl_state_t;

char ***krill_split_line(const char *line, repl_state_t *repl_state);
void krill_parse_line(token_state_t *state, const char *line);
void krill_continue_quotes(token_state_t *state, repl_state_t *repl_state);
void krill_continue_pipe(token_state_t *state, repl_state_t *repl_state);
void handle_pipe(token_state_t *state);
void handle_space(token_state_t *state);