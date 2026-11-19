#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "read_line.h"
#include "terminal_mode.h"

#define LSH_RL_BUFSIZE 4096
#define CLEAR_LINE "\x1b[2K\r"

char *lsh_read_line(repl_state_t *state) {
    char *line = calloc(LSH_RL_BUFSIZE, sizeof(char));

    if (!line) {
        fprintf(stderr, "\nlsh: allocation error\n");
        return NULL;
    }

    char *saved_line = calloc(LSH_RL_BUFSIZE, sizeof(char));

    if (!saved_line) {
        fprintf(stderr, "\nlsh: allocation error\n");
        free(line);
        return NULL;
    }
    
    state->current_line = line;
    state->line_position = -1;
    state->saved_line = saved_line;
    int line_size = 2;

    enable_raw_mode();
    char c;
    char seq[3];

    fputs(state->prompt, stdout);
    fflush(stdout);
    while (1) {
        while (read(STDIN_FILENO, &c, 1) != 1);

        switch (c)
        {
        case '\033':

            if (read(STDIN_FILENO, &seq[0], 1) != 1) continue;
            if (read(STDIN_FILENO, &seq[1], 1) != 1) continue;

            if (seq[0] == '[') {
                switch (seq[1])
                {
                case 'A':
                    if (state->history_position != 0) {
                        if (state->history[state->history_position] == NULL) {
                            strcpy(state->saved_line, state->current_line);
                        }
                        
                        state->history_position--;
                        strcpy(state->current_line, state->history[state->history_position]);
                        state->line_position = strlen(state->history[state->history_position]) - 1;
                        line_size = strlen(state->current_line) + 2;
                        
                        printf("%s%s%s", CLEAR_LINE, state->prompt, state->current_line);
                        fflush(stdout);
                    }
                    break;
                case 'B':
                    if (state->history[state->history_position + 1] != NULL && state->history[state->history_position] != NULL) {
                        state->history_position++;
                        strcpy(state->current_line, state->history[state->history_position]);
                        
                        state->line_position = strlen(state->history[state->history_position]) - 1;
                        line_size = strlen(state->current_line) + 2;
                    } else if (state->history[state->history_position + 1] == NULL && state->history[state->history_position] != NULL) {
                        state->history_position++;
                        strcpy(state->current_line, state->saved_line);
                        
                        state->line_position = strlen(state->saved_line) - 1;
                        line_size = strlen(state->current_line) + 2;  
                    }
                    
                    printf("%s%s%s", CLEAR_LINE, state->prompt, state->current_line);
                    fflush(stdout);
                    break;
                }
            }
            break;
        case '\x03':
            disable_raw_mode();
            puts("^C");
            
            free(state->current_line);
            free(state->saved_line);
            state->saved_line = NULL;
            
            exit(EXIT_SUCCESS);
        case '\n':
        case '\r':
            disable_raw_mode();
            fputs("\n", stdout);
            
            if (strlen(state->current_line) != 0) {
                while (state->history[state->history_position] != NULL) {
                    state->history_position++;
                }
                state->history[state->history_position] = calloc(strlen(state->current_line) + 1, sizeof(char));
                strcpy(state->history[state->history_position], state->current_line);
                state->history_position++;
                state->history[state->history_position] = NULL;
            }
            
            state->line_position++;
            state->current_line[state->line_position] = '\n';
            state->current_line[state->line_position + 1] = '\0';
            free(state->saved_line);
            state->saved_line = NULL;
            
            return state->current_line;
        case '\x7f':
        case '\x08':
            if (strlen(state->current_line) > 0 && state->line_position != -1) {
                state->current_line[state->line_position] = '\0';
                state->line_position--;
                line_size = strlen(state->current_line);
                
                printf("%s%s%s", CLEAR_LINE, state->prompt, state->current_line);
                fflush(stdout);
            }
            break;
        default:
            if (c >= 32 && c <= 126 && line_size < LSH_RL_BUFSIZE) {
                state->line_position++;
                state->current_line[state->line_position] = c;
                state->current_line[state->line_position+1] = '\0';
                line_size = strlen(state->current_line);
                
                printf("%s%s%s", CLEAR_LINE, state->prompt, state->current_line);
                fflush(stdout);
            }
            break;
        }
    }
}