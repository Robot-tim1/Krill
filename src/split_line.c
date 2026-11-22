#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "split_line.h"
#include "read_line.h"

#define TOK_BUFSIZE 64

char **krill_split_line(const char *line, repl_state_t *repl_state) {
    char **tokens = malloc(TOK_BUFSIZE * sizeof(char *));

    if (!tokens) {
        fprintf(stderr, "Krill: allocation error\n");
        return NULL;
    }

    token_state_t state = {
            .token = NULL,
            .tokens = tokens,
            .token_bufsize = TOK_BUFSIZE,
            .tokens_bufsize = TOK_BUFSIZE,
            .token_position = 0,
            .word_position = 0,
            .is_double_quoted = 0,
            .is_single_quoted = 0
    };

    krill_parse_line(&state, line);

    if (state.is_double_quoted || state.is_single_quoted) {
        krill_handle_quotes(&state, repl_state);
    }

    if (state.token) {
        state.token[state.token_position] = '\0';
        state.tokens[state.word_position] = state.token;
        state.word_position++;
    }

    state.tokens[state.word_position] = NULL;
    return state.tokens;
}

void krill_handle_quotes(token_state_t *state, repl_state_t *repl_state) {
    char *line;
    char *temp = repl_state->prompt;
    repl_state->prompt = "> ";

    while (state->is_double_quoted || state->is_single_quoted) {
        line = krill_read_line(repl_state);
        krill_parse_line(state, line);
        free(line);
    }

    repl_state->prompt = temp;
}

void krill_parse_line(token_state_t *state, const char *line) {
    int line_position = 0;

    while (line[line_position] != '\0') {
        if (line[line_position] == '"' && !state->is_single_quoted) {
            if (state->is_double_quoted) {
                state->is_double_quoted = 0;
            } else {
                state->is_double_quoted = 1;
            }
            line_position++;
            continue;
        }

        // 39 is single quote ASCII number
        if (line[line_position] == 39 && !state->is_double_quoted) {
            if (state->is_single_quoted) {
                state->is_single_quoted = 0;
            } else {
                state->is_single_quoted = 1;
            }
            line_position++;
            continue;
        }

        if (isspace(line[line_position]) && !state->is_double_quoted && !state->is_single_quoted) {
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
                state->tokens_bufsize += TOK_BUFSIZE;
                char **temp = realloc(state->tokens, state->tokens_bufsize * sizeof(char *));

                if (!temp) {
                    fprintf(stderr, "Krill: allocation error\n");
                    exit(EXIT_FAILURE);
                }
                state->tokens = temp;
            }

            state->token = NULL;
            state->token_bufsize = TOK_BUFSIZE;
            continue;
        }

        if (!state->token) {
            state->token = malloc(state->token_bufsize);
            if (!state->token) {
                fprintf(stderr, "Krill: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        state->token[state->token_position] = line[line_position];
        line_position++;
        state->token_position++;

        if (state->token_position + 1 >= state->token_bufsize) {
            state->token_bufsize += TOK_BUFSIZE;
            char *temp = realloc(state->token, state->token_bufsize);

            if (!temp) {
                fprintf(stderr, "Krill: allocation error\n");
                exit(EXIT_FAILURE);
            }
            state->token = temp;
        }
    }
}