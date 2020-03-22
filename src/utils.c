#include "../include/utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

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

int get_num_digits(long int n) {
    int res = 1;
    while (n = n / 10, n) {
        ++res;
    }
    return res;
}

void print_options(Options *opt) {
    printf("Current path: %s\n", opt->path);
    printf("Block size: %ld\n", opt->block_size);
    printf("Flags:\n");
    printf("\t--all (-a): %d\n", opt->all);
    printf("\t'apparent_size' (from -b or --bytes): %d\n", opt->apparent_size);
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
    opt->apparent_size = false;

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
                opt->apparent_size = 1;
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
                    fprintf(stderr, "Argument -B requires an additional integer argument\n");
                    return 1;
                }
                else if (!is_num(argv[cur_arg])) {
                    fprintf(stderr, "Parameter must be an integer (given \"%s\")\n", argv[cur_arg]);
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
                    fprintf(stderr, "Argument must be an integer (given \"%s\")\n", argv[cur_arg] + BLOCK_SIZE_STR_OFFSET);
                    return 1;
                }
            }
            else if (strstr(argv[cur_arg], "--max-depth=") == argv[cur_arg]) {
                if (is_num(argv[cur_arg] + MAX_DEPTH_STR_OFFSET)) {
                    opt->max_depth = true;
                    opt->depth_val = atoi(argv[cur_arg] + MAX_DEPTH_STR_OFFSET);
                }
                else {
                    fprintf(stderr, "Argument must be an integer (given \"%s\")\n", argv[cur_arg] + MAX_DEPTH_STR_OFFSET);
                    return 1;
                }
            }
            else if (strcmp(argv[cur_arg], "--apparent-size") == 0) {
                opt->apparent_size = true;
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

void exec_next_dir(char *complete_path, Options *opt, char *envp[]) {

    // Max num of args is 10
    char *vec[10];
    int i = 2;

    vec[0] = APPLICATION_NAME;
    vec[1] = complete_path;

    if (opt->all) {
        vec[i] = "-a";
        ++i;
    }

    if (opt->apparent_size) {
        vec[i] = "--apparent-size";
        ++i;
    }

    if (opt->dereference) {
        vec[i] = "-L";
        ++i;
    }

    if (opt->separate_dirs) {
        vec[i] = "-S";
        ++i;
    }

    if (opt->block_size != DEFAULT_BLOCK_SIZE) {
        vec[i] = "-B";
        ++i;
        int len = get_num_digits(opt->block_size);
        vec[i] = calloc(len + 1, sizeof(*vec[i]));
        sprintf(vec[i], "%ld", opt->block_size);
        ++i;
    }

    if (opt->max_depth) {
        int depth_val = opt->depth_val - 1;
        depth_val = depth_val < 0 ? 0 : depth_val;

        int len = get_num_digits(depth_val);
        vec[i] = calloc(len + 1 + 12, sizeof(*vec[i]));
        vec[i] = "--max-depth=";
        sprintf(vec[i], "%ld", opt->block_size);
        ++i;
    }

    vec[i] = NULL;
    execve(vec[0], vec, envp);
}
