#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

#include "terminal_mode.h"

static struct termios orig_termios;
static int termios_saved = 0;

void disable_raw_mode(void) {
    if (termios_saved) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    }
}

void enable_raw_mode(void) {
    if (!termios_saved) {
        tcgetattr(STDIN_FILENO, &orig_termios);
        atexit(disable_raw_mode);
        termios_saved = 1;
    }

    struct termios raw = orig_termios;

    raw.c_lflag &= ~(ICANON | ECHO | IEXTEN | ISIG);
    raw.c_iflag &= ~(IXON | ICRNL);

    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}