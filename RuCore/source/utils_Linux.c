#include <rukky/core.h>

#include <stdio.h>

void fatal(const char *msg) {
    fprintf(stderr, "Fatal: %s\n", msg);
    exit(1);
}