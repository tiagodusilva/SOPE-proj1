#include "../include/showDirec.h"

int showDirec(Options * opt){
    DIR * direc;
    struct dirent * dirent;
    if ((direc = opendir(opt->path)) == NULL){
        fprintf(stderr, "Not possible to open directory\n");
        return 1;
    }

    while((dirent = readdir(direc)) != NULL){
        printf("%-25s", dirent->d_name);  
        if (printFileState(opt, dirent->d_name))  
            return 1; 

    }

    if (closedir(direc) == -1){
        fprintf(stderr, "Not possible to close directory\n");
        return 1;
    }

    return 0;
}

int printFileState(Options* opt, char *name){
    struct stat s; 
    long int numBlocks; 

    char completePath[PATH_SIZE_MAX] = ""; 
    strcpy(completePath, opt->path);
    strcat(completePath, "/");
    strcat(completePath, name);  


    if (stat(completePath, &s) < 0){
        fprintf(stderr, "Not possible to get file stat\n"); 
        return 1; 
    }

    numBlocks = s.st_size/(opt->block_size)+1; 

    if (opt->block_size == 1) printf("%ld\n", s.st_size); 
    else printf("%ld\n", numBlocks); 

    return 0; 
}