#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "split_line.h"
#include "read_line.h"

#define LSH_TOK_BUFSIZE 64

char **lsh_split_line(const char *line, repl_state_t *repl_state) {
    char **tokens = malloc(LSH_TOK_BUFSIZE * sizeof(char *));

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        return NULL;
    }

    token_state_t state = {
            .token = NULL,
            .tokens = tokens,
            .token_bufsize = LSH_TOK_BUFSIZE,
            .tokens_bufsize = LSH_TOK_BUFSIZE,
            .token_position = 0,
            .word_position = 0,
            .is_quoted = 0
    };

    lsh_parse_line(&state, line);

    if (state.is_quoted) {
        lsh_handle_quotes(&state, repl_state);
    }

    if (state.token) {
        state.token[state.token_position] = '\0';
        state.tokens[state.word_position] = state.token;
        state.word_position++;
    }

    state.tokens[state.word_position] = NULL;
    return state.tokens;
}

void lsh_handle_quotes(token_state_t *state, repl_state_t *repl_state) {
    char *line;

    while (state->is_quoted) {
        printf("> ");
        line = lsh_read_line(repl_state);
        lsh_parse_line(state, line);
        free(line);
    }
}

void lsh_parse_line(token_state_t *state, const char *line) {
    int line_position = 0;

    while (line[line_position] != '\0') {
        if (line[line_position] == '"') {
            if (state->is_quoted) {
                state->is_quoted = 0;
            } else {
                state->is_quoted = 1;
            }
            line_position++;
            continue;
        }

        if (isspace(line[line_position]) && !state->is_quoted) {
            if (state->token_position == 0) {
                line_position++;
                continue;
            }

            state->token[state->token_position] = '\0';
            line_position++;
            state->token_position = 0;

            state->tokens[state->word_position] = state->token;
            state->word_position++;

            if (state->word_position + 1 >= state->tokens_bufsize) {
                state->tokens_bufsize += LSH_TOK_BUFSIZE;
                char **temp = realloc(state->tokens, state->tokens_bufsize * sizeof(char *));

                if (!temp) {
                    fprintf(stderr, "lsh: allocation error\n");
                    exit(EXIT_FAILURE);
                }
                state->tokens = temp;
            }

            state->token = NULL;
            state->token_bufsize = LSH_TOK_BUFSIZE;
            continue;
        }

        if (!state->token) {
            state->token = malloc(state->token_bufsize);
            if (!state->token) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        state->token[state->token_position] = line[line_position];
        line_position++;
        state->token_position++;

        if (state->token_position + 1 >= state->token_bufsize) {
            state->token_bufsize += LSH_TOK_BUFSIZE;
            char *temp = realloc(state->token, state->token_bufsize);

            if (!temp) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
            state->token = temp;
        }
    }
}