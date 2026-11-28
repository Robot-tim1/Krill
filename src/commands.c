#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "commands.h"

char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &krill_cd,
  &krill_help,
  &krill_exit
};

int krill_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

int krill_execute(char ***commands) {
    if (commands[0][0] == NULL) {
        return 1;
    }

    int i;
    if (commands[1] == NULL) {
        char **args = commands[0];
        for (i = 0; i < krill_num_builtins(); i++) {
            if (strcmp(args[0], builtin_str[i]) == 0) {
                return (*builtin_func[i])(args);
            }
        }
    }

    int in_fd = STDIN_FILENO;

    for (i = 0; commands[i] != NULL; i++) {
        char **args = commands[i];
        bool builtin = false;
        
        int fd[2];
        if (pipe(fd) == -1) {
            perror("Krill");
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < krill_num_builtins(); j++) {
            if (strcmp(args[0], builtin_str[j]) == 0) {
                builtin = true;
                break;
            }
        }

        krill_launch(args, fd, &in_fd, commands[i+1] == NULL, builtin);
    }

    close(in_fd);

    for (int i = 0; commands[i] != NULL; i++) {
        wait(NULL);
    }

    return 1;
}

int krill_launch(char **args, int *fd, int *in_fd, bool is_last_command, bool is_builtin) {
    pid_t pid = fork();
    if (pid == 0) {
        if (*in_fd != STDIN_FILENO) {
            dup2(*in_fd, STDIN_FILENO);
            close(*in_fd);
        }

        if (!is_last_command) {
            dup2(fd[1], STDOUT_FILENO);
        }

        close(fd[0]);
        close(fd[1]);
        
        if (is_builtin) {
            for (int i = 0; i < krill_num_builtins(); i++) {
                if (strcmp(args[0], builtin_str[i]) == 0) {
                    (*builtin_func[i])(args);
                    break;
                }
            }
        } else {
            if (execvp(args[0], args) == -1) {
                perror("Krill");
            }
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Krill");
    }

    if (*in_fd != STDIN_FILENO) {
        close(*in_fd);
    }

    *in_fd = fd[0];
    close(fd[1]);
    
    return 1;
}

int krill_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Krill: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("Krill");
        }
    }
    return 1;
}

int krill_help(char **args) {
    int i;
    printf("Krill started as Stephen Brennan's LSH\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (i = 0; i < krill_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int krill_exit(char **args) {
    return 0;
}