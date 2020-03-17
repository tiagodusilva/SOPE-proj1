#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "utils.h"

int parse_arguments(int argc, char *argv[], Options *opt) {
    if (argc == 1) {
        opt->path = ".";
        return 0;
    }
    else {
        // Do stuff
    }
    return 1;
}

int main(int argc, char *argv[], char *envp[]) {
    Options opt;
    if (parse_arguments(argc, argv, &opt)) {

    }
    printf("Current path: %s\n", opt.path);
    exit(0);
}