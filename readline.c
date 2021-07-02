#if defined(_WIN32) || defined(__EMSCRIPTEN__)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char buffer[2048];

char *readline(const char *prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char *cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) - 1] = '\0';
    return cpy;
}

void add_history(char *unused) {}

#else
#include <editline/readline.h>
#endif
