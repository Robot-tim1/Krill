#pragma once

int krill_launch(char **args, int *fd, int *in_fd, int is_last_command);
int krill_execute(char ***commands);
int krill_cd(char **args);
int krill_help(char **args);
int krill_exit(char **args);