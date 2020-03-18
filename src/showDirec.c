#include "../include/showDirec.h"

int showDirec(Options * opt){
    DIR * direc;
    struct dirent * dirent;
    if ((direc = opendir(opt->path)) == NULL){  
        fprintf(stderr, "Not possible to open directory\n");
        return 1;
    }

    while((dirent = readdir(direc)) != NULL){
        //do not show the .. directory
        if (strcmp(dirent->d_name, "..") == 0) continue;
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

    //get the complete path of the file called "name"
    char completePath[PATH_SIZE_MAX] = ""; 
    strncpy(completePath, opt->path, PATH_MAX_CPY);
    strncat(completePath, "/", PATH_MAX_CPY);
    strncat(completePath, name, PATH_MAX_CPY);  


    if (stat(completePath, &s) < 0){
        fprintf(stderr, "Not possible to get file stat\n"); 
        return 1; 
    }

    //get the number of blocks allocated for the file according to the OS configurations
    numBlocks = s.st_size/(opt->block_size)+1; 

    //prints the size information according to the options
    if (opt->block_size == 1) printf("%ld\n", s.st_size); 
    else printf("%ld\n", numBlocks); 

    return 0; 
}