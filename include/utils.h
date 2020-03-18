#pragma once

#include <stdbool.h>

#define DEFAULT_BLOCK_SIZE 1024
#define BLOCK_SIZE_STR_OFFSET 12
#define MAX_DEPTH_STR_OFFSET 11

typedef struct cmd_options {
    bool all, dereference, separate_dirs, max_depth;
    int block_size, depth_val;
    char *path;
} Options;

int is_num(char *s);
