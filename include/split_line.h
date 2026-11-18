#pragma once

typedef struct TokenState {
    char *token;
    char **tokens;
    int word_position;
    int token_position;
    int is_quoted;
    int token_bufsize;
    int tokens_bufsize;
} token_state_t;

typedef struct ReplState repl_state_t;

char **lsh_split_line(const char *line, repl_state_t *repl_state);
void lsh_parse_line(token_state_t *state, const char *line);
void lsh_handle_quotes(token_state_t *state, repl_state_t *repl_state);