#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

#include "lsh.h"
#include "split_line.h"
#include "commands.h"

#define LSH_RL_BUFSIZE 1024

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

#define MAX_PATH 4096

#define HRED    "\e[0;91m"
#define RED     "\e[0;31m"
#define RESET   "\e[0m"

void lsh_loop(void) {
    char *line;
    char **args;
    int status;
    char hostname[256];

    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("gethostname() error");
        return;
    }

    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);

    if (pw == NULL) {
        perror("getpwuid() error");
        return;
    }

    do {  
        char cwd[MAX_PATH];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd() error");
            return;
        }
        printf("%s%s@%s%s:%s%s%s$ ", RED, pw->pw_name, hostname, RESET, HRED, cwd, RESET);
        line = lsh_read_line();
        args = lsh_split_line(line);
        status = lsh_execute(args);

        free(line);
        for (int i = 0; args[i] != NULL; i++) {
            free(args[i]);
        }
        free(args);
    } while (status);
}

char *lsh_read_line(void) {
    int bufsize = LSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        c = getchar();

        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        if (position >= bufsize) {
            bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}