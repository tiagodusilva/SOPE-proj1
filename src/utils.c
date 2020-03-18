#include "../include/utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

bool is_num(char *s) {
    if (*s == '\0')
        return false;
    while (*s != '\0') {
        if (*s < '0' || *s > '9')
            return false;
        ++s;
    }
    return true;
}

void print_options(Options *opt) {
    printf("Current path: %s\n", opt->path);
    printf("Block size: %d\n", opt->block_size);
    printf("Flags:\n");
    printf("\t--all (-a): %d\n", opt->all);
    printf("\t--dereference (-L): %d\n", opt->dereference);
    printf("\t--separate-dirs (-S): %d\n", opt->separate_dirs);
    if (!opt->max_depth)
        printf("\t--max-depth: %d\n", opt->max_depth);
    else
        printf("\t--max-depth: %d\n", opt->depth_val);
}

// Flags:
// a (all), b (bytes), -L (dereference), S (separate-dirs)
// B (block-size=), max-depth=N
int parse_arguments(int argc, char *argv[], Options *opt) {
    
    if (opt == NULL)
        return 1;

    // Default params
    opt->path = ".";
    opt->block_size = DEFAULT_BLOCK_SIZE;
    opt->all = false;
    opt->dereference = false;
    opt->max_depth = false;
    opt->separate_dirs = false;

    if (argc <= 0) {
        return 1;
    }
    else {
        int cur_arg = 1;
        while (cur_arg < argc) {
            if (strcmp(argv[cur_arg], "-a") == 0 || strcmp(argv[cur_arg], "--all") == 0) {
                opt->all = true;
            }
            else if (strcmp(argv[cur_arg], "-b") == 0 || strcmp(argv[cur_arg], "--bytes") == 0) {
                opt->block_size = 1;
            }
            else if (strcmp(argv[cur_arg], "-L") == 0 || strcmp(argv[cur_arg], "--dereference") == 0) {
                opt->dereference = true;
            }
            else if (strcmp(argv[cur_arg], "-S") == 0 || strcmp(argv[cur_arg], "--separate-dirs") == 0) {
                opt->separate_dirs = true;
            }
            else if (strcmp(argv[cur_arg], "-B") == 0) {
                ++cur_arg;
                if (cur_arg >= argc) {
                    printf("Argument -B requires an additional integer argument\n");
                    return 1;
                }
                else if (!is_num(argv[cur_arg])) {
                    printf("Parameter must be an integer (given \"%s\")\n", argv[cur_arg]);
                    return 1;
                }
                else {
                    opt->block_size = atoi(argv[cur_arg]);
                    opt->block_size = opt->block_size >= 1 ? opt->block_size : 1;
                }
            }
            else if (strstr(argv[cur_arg], "--block-size=") == argv[cur_arg]) {
                if (is_num(argv[cur_arg] + BLOCK_SIZE_STR_OFFSET)) {
                    opt->block_size = atoi(argv[cur_arg] + BLOCK_SIZE_STR_OFFSET);
                    opt->block_size = opt->block_size >= 1 ? opt->block_size : 1;
                }
                else
                {
                    printf("Argument must be an integer (given \"%s\")\n", argv[cur_arg] + BLOCK_SIZE_STR_OFFSET);
                    return 1;
                }
            }
            else if (strstr(argv[cur_arg], "--max-depth=") == argv[cur_arg]) {
                if (is_num(argv[cur_arg] + MAX_DEPTH_STR_OFFSET)) {
                    opt->max_depth = atoi(argv[cur_arg] + MAX_DEPTH_STR_OFFSET);
                }
                else {
                    printf("Argument must be an integer (given \"%s\")\n", argv[cur_arg] + MAX_DEPTH_STR_OFFSET);
                    return 1;
                }
            }
            else {
                // Assume it's the directory path
                opt->path = argv[cur_arg];
            }

            ++cur_arg;
        }
    }

    return 0;
}
