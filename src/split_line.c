#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "split_line.h"

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

char **lsh_split_line(char *line) {
    int tokens_bufsize = LSH_TOK_BUFSIZE;
    int token_bufsize = LSH_TOK_BUFSIZE;
    int token_position = 0, word_position = 0, line_position = 0;
    char **tokens = malloc(tokens_bufsize * sizeof(char *));
    char *token = NULL;

    int is_quoted = 0;

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (line[line_position] != '\0') {
        if (line[line_position] == '"') {
            if (is_quoted) {
                is_quoted = 0;
            } else {
                is_quoted = 1;
            }
            line_position++;
            continue;
        }

        if (isspace(line[line_position]) && !is_quoted) {
            if (token_position == 0) {
                line_position++;
                continue;
            }

            token[token_position] = '\0';
            line_position++;
            token_position = 0;

            tokens[word_position] = token;
            word_position++;

            if (word_position + 1 >= tokens_bufsize) {
                tokens_bufsize += LSH_TOK_BUFSIZE;
                char **temp = realloc(tokens, tokens_bufsize * sizeof(char *));

                if (!temp) {
                    fprintf(stderr, "lsh: allocation error\n");
                    exit(EXIT_FAILURE);
                }
                tokens = temp;
            }

            token = NULL;
            token_bufsize = LSH_TOK_BUFSIZE;
            continue;
        }

        if (!token) {
            token = malloc(token_bufsize);
            if (!token) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token[token_position] = line[line_position];
        line_position++;
        token_position++;

        if (token_position + 1 >= token_bufsize) {
            token_bufsize += LSH_TOK_BUFSIZE;
            char *temp = realloc(token, token_bufsize);

            if (!temp) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
            token = temp;
        }
    }

    if (token) {
        token[token_position] = '\0';
        tokens[word_position] = token;
        word_position++;
    }

    tokens[word_position] = NULL;
    return tokens;
}