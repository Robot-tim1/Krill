#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "split_line.h"
#include "read_line.h"

#define TOK_BUFSIZE 64

char ***krill_split_line(const char *line, repl_state_t *repl_state) {
    char **tokens = malloc(TOK_BUFSIZE * sizeof(char *));

    if (!tokens) {
        fprintf(stderr, "Krill: allocation error\n");
        return NULL;
    }

    char ***commands = malloc(TOK_BUFSIZE * sizeof(char **));

    if (!commands) {
        fprintf(stderr, "Krill: allocation error\n");
        return NULL;
    }

    token_state_t state = {
            .token = NULL,
            .tokens = tokens,
            .commands = commands,
            .token_bufsize = TOK_BUFSIZE,
            .tokens_bufsize = TOK_BUFSIZE,
            .command_bufsize = TOK_BUFSIZE,
            .token_position = 0,
            .word_position = 0,
            .command_position = 0,
            .is_double_quoted = 0,
            .is_single_quoted = 0
    };

    krill_parse_line(&state, line);

    if (state.is_double_quoted || state.is_single_quoted || (!state.token && state.word_position == 0 && state.command_position != 0)) {
        char *temp = repl_state->prompt;
        repl_state->prompt = "> ";
        repl_state->secondary = true;
        
        while (state.is_double_quoted || state.is_single_quoted || (!state.token && state.word_position == 0 && state.command_position != 0)) {
            if (state.is_double_quoted || state.is_single_quoted) {
                krill_continue_quotes(&state, repl_state);
            }
            if (!state.token && state.word_position == 0 && state.command_position != 0) {
                krill_continue_pipe(&state, repl_state);
            }
        }
        
        repl_state->secondary = false;
        repl_state->prompt = temp;
    }

    if (state.token) {
        state.token[state.token_position] = '\0';
        state.tokens[state.word_position] = state.token;
        state.word_position++;
    }

    state.tokens[state.word_position] = NULL;
    state.commands[state.command_position] = state.tokens;
    state.commands[state.command_position+1] = NULL;
    return state.commands;
}

void krill_continue_quotes(token_state_t *state, repl_state_t *repl_state) {
    char *line;
    repl_state->add_newline = true;

    if (!state->token) {
        state->token = malloc(state->token_bufsize);
        if (!state->token) {
            fprintf(stderr, "Krill: allocation error\n");
            exit(EXIT_FAILURE);
        }
    }

    state->token[state->token_position] = '\n';
    state->token_position++;
    while (state->is_double_quoted || state->is_single_quoted) {
        line = krill_read_line(repl_state);
        if (!line) {
            continue;
        }
        krill_parse_line(state, line);
        if (state->is_double_quoted || state->is_single_quoted) {
            state->token[state->token_position] = '\n';
            state->token_position++;
        }
        free(line);
    }

    repl_state->add_newline = false;
}

void krill_continue_pipe(token_state_t *state, repl_state_t *repl_state) {
    char *line;
    repl_state->add_space = true;

    while (!state->token && state->word_position == 0 && state->command_position != 0) {
        line = krill_read_line(repl_state);
        if (!line) {
            continue;
        }
        krill_parse_line(state, line);
        free(line);
    }

    repl_state->add_space = false;
}

void krill_parse_line(token_state_t *state, const char *line) {
    int line_position = 0;

    while (line[line_position] != '\0') {
        if (line[line_position] == '"' && !state->is_single_quoted) {
            if (state->is_double_quoted) {
                state->is_double_quoted = 0;
                if (state->token == NULL) {
                    state->token = malloc(state->token_bufsize);
                    if (!state->token) {
                        fprintf(stderr, "Krill: allocation error\n");
                        exit(EXIT_FAILURE);
                    }
                    add_token(state);
                }
            } else {
                state->is_double_quoted = 1;
            }
            line_position++;
            continue;
        }

        if (line[line_position] == '\'' && !state->is_double_quoted) {
            if (state->is_single_quoted) {
                state->is_single_quoted = 0;
                if (!state->token) {
                    state->token = malloc(state->token_bufsize);
                    if (!state->token) {
                        fprintf(stderr, "Krill: allocation error\n");
                        exit(EXIT_FAILURE);
                    }
                    add_token(state);
                }
            } else {
                state->is_single_quoted = 1;
            }
            line_position++;
            continue;
        }

        if (isspace(line[line_position]) && !state->is_double_quoted && !state->is_single_quoted) {
            line_position++;
            if (!state->token) {
                continue;
            }
            add_token(state);
            continue;
        }

        if (line[line_position] == '|' && !state->is_double_quoted && !state->is_single_quoted) {
            line_position++;
            handle_pipe(state);
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

void add_token(token_state_t *state) {
    state->token[state->token_position] = '\0';
    
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
    state->token_position = 0;
    state->token_bufsize = TOK_BUFSIZE;
}

void handle_pipe(token_state_t *state) {
    if (state->token) {
        add_token(state);
    }

    state->tokens[state->word_position] = NULL;

    if (state->tokens[0] == NULL) {
        fprintf(stderr, "Krill: syntax error near unexpected token `|'\n");
        exit(EXIT_FAILURE);
    }

    state->commands[state->command_position] = state->tokens;
    state->command_position++;

    if (state->command_position + 1 >= state->command_bufsize) {
        state->command_bufsize += TOK_BUFSIZE;
        char ***temp = realloc(state->commands, state->command_bufsize * sizeof(char **));

        if (!temp) {
            fprintf(stderr, "Krill: allocation error\n");
            exit(EXIT_FAILURE);
        }
        state->commands = temp;
    }

    state->tokens_bufsize = TOK_BUFSIZE;
    state->word_position = 0;

    char **tokens = malloc(state->tokens_bufsize * sizeof(char *));

    if (!tokens) {
        fprintf(stderr, "Krill: allocation error\n");
        exit(EXIT_FAILURE);
    }

    state->tokens = tokens;
}