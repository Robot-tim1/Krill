#pragma once

#include <stdbool.h>

int krill_launch(char **args, int *fd, int *in_fd, bool is_last_command, bool is_builtin);
int krill_execute(char ***commands);
int krill_cd(char **args);
int krill_help(char **args);
int krill_exit(char **args);