#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

#include "lsh.h"
#include "split_line.h"
#include "commands.h"
#include "read_line.h"
#include "history_queue.h"

#define _GNU_SOURCE

#define MAX_PATH 4096

#define HRED    "\e[0;91m"
#define RED     "\e[0;31m"
#define RESET   "\e[0m"

void lsh_loop(void) {
    char *line;
    char **args;
    int status = 1;
    char hostname[256];
    char *home = getenv("HOME");

    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("lsh");
        return;
    }

    struct passwd *pw = getpwuid(geteuid());

    if (!pw) {
        perror("lsh");
        return;
    }

    queue_t *history = create_queue(1024);

    if (!history) {
        perror("lsh");
        return;
    }

    repl_state_t repl_state = {
        .history = history,
        .current_line = NULL,
        .saved_line = NULL,
        .yanked_line = NULL,
        .prompt = NULL,
        .line_position = -1,
        .history_position = -1,
        .line_length = 0
    };

    do {  
        char cwd[MAX_PATH];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("lsh");
            return;
        }
        
        char *cwd_to_print = cwd;
        if (strncmp(home, cwd, strlen(home)) == 0) {
            char new_cwd[MAX_PATH];
            memset(new_cwd, 0, sizeof(new_cwd));
            
            new_cwd[0] = '~';
            strcat(new_cwd, cwd + strlen(home));
            
            cwd_to_print = new_cwd;
        }
        if (asprintf(&repl_state.prompt, "%s%s@%s%s:%s%s%s$ ", RED, pw->pw_name, hostname, RESET, HRED, cwd_to_print, RESET) == -1) {
            perror("lsh");
            return;
        }
        
        line = lsh_read_line(&repl_state);
        if (!line) {
            free(repl_state.prompt);
            repl_state.prompt = NULL;
            continue;
        }
        args = lsh_split_line(line, &repl_state);
        if (!args) {
            free(line);
            free(repl_state.prompt);
            repl_state.prompt = NULL;
            continue;
        }
        status = lsh_execute(args);

        free(line);
        for (int i = 0; args[i] != NULL; i++) {
            free(args[i]);
        }
        free(args);
        free(repl_state.prompt);
        repl_state.prompt = NULL;
    } while (status);
}