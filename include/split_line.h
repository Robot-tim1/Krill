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

char **lsh_split_line(char *line);
void lsh_parse_line(token_state_t *state, char *line);
void lsh_handle_quotes(token_state_t *state);