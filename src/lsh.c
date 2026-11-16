#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

#include "lsh.h"
#include "split_line.h"
#include "commands.h"
#include "read_line.h"

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
        perror("lsh");
        return;
    }

    struct passwd *pw = getpwuid(geteuid());

    if (pw == NULL) {
        perror("lsh");
        return;
    }

    do {  
        char cwd[MAX_PATH];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("lsh");
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