#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "../include/utils.h"

// Flags:
// a (all), b (bytes), -L (dereference), S (separate-dirs)
// B (block-size=), max-depth=N
int parse_arguments(int argc, char *argv[], Options *opt) {
    
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
        int cur_arg = 2;
        while (cur_arg <= argc) {
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
                if (cur_arg > argc) {
                    printf("Argument -B requires an additional integer argument\n");
                    return 1;
                }
                else if (!is_num(argv[cur_arg])) {
                    printf("Parameter must be an integer\n");
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
                    printf("Argument must be an integer\n");
                    return 1;
                }
            }
            else if (strstr(argv[cur_arg], "--max-depth=") == argv[cur_arg]) {
                if (is_num(argv[cur_arg] + MAX_DEPTH_STR_OFFSET)) {
                    opt->max_depth = atoi(argv[cur_arg] + MAX_DEPTH_STR_OFFSET);
                }
                else {
                    printf("Argument must be an integer\n");
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



int main(int argc, char *argv[], char *envp[]) {
    Options opt;
    if (parse_arguments(argc, argv, &opt)) {
        printf("Invalid command\n");
        exit(1);
    }

    printf("Current path: %s\n", opt.path);
    exit(0);
}