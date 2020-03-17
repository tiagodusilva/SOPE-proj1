#pragma once

#include <stdbool.h>

typedef struct cmd_options {
    bool all, bytes, block_size, dereference, separate_dirs, max_depth;
    int block_val, depth_val;
    char *path;
} Options;
