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

int krill_execute(char **args) {
    int i;

    if (args[0] == NULL) {
        return 1;
    }

    for (i = 0; i < krill_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return krill_launch(args);
}

int krill_launch(char **args) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("Krill");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Krill");
    } else {
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    
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