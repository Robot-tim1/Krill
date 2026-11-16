#pragma once

void lsh_loop(void);
char *lsh_read_line(void);
int lsh_launch(char **args);
int lsh_execute(char **args);
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);