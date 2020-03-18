#pragma once

#include <stdbool.h>

#define DEFAULT_BLOCK_SIZE 1024
#define BLOCK_SIZE_STR_OFFSET 13
#define MAX_DEPTH_STR_OFFSET 12

typedef struct cmd_options {
    bool all, dereference, separate_dirs, max_depth;
    int block_size, depth_val;
    char *path;
} Options;

bool is_num(char *s);

// Flags:
// a (all), b (bytes), -L (dereference), S (separate-dirs)
// B (block-size=), max-depth=N
int parse_arguments(int argc, char *argv[], Options *opt);

void print_options(Options *opt);
