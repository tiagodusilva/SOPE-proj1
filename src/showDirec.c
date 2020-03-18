#include "../include/showDirec.h"

int showDirec(Options * opt){
    DIR * direc;
    struct dirent * dirent;
    if ((direc = opendir(opt->path)) == NULL){
        fprintf(stderr, "Not possible to open directory\n");
        exit(1);
    }

    while((dirent = readdir(direc)) != NULL)
        printf("%s\t\n", dirent->d_name);

    if (closedir(direc) == -1){
        fprintf(stderr, "Not possible to close directory\n");
        exit(1);
    }

    exit(0); 
}