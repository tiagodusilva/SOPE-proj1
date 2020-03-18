#include "../include/showDirec.h"

static inline void print_file(long int size, char *s) {
    printf("%-8ld%s\n", size, s);
}

int showDirec(Options * opt){
    DIR * direc;
    struct dirent * dirent;
    long int dir_size = 0, tmp;

    if ((direc = opendir(opt->path)) == NULL){  
        fprintf(stderr, "Not possible to open directory\n");
        return 1;
    }

    while((dirent = readdir(direc)) != NULL){
        if (tmp = analyze_file(opt, dirent->d_name), tmp == -1)  
            return 1;
        dir_size += tmp;
    }

    if (closedir(direc) == -1){
        fprintf(stderr, "Not possible to close directory\n");
        return 1;
    }

    print_file(dir_size, opt->path);

    return 0;
}

long int analyze_file(Options* opt, char *name){
    struct stat st; 
    long int size; 

    //get the complete path of the file called "name"
    char completePath[PATH_SIZE_MAX] = ""; 
    strcpy(completePath, opt->path);
    strcat(completePath, "/");
    strcat(completePath, name);

    if (stat(completePath, &st) < 0){
        fprintf(stderr, "Not possible to get file stat\n"); 
        return -1; 
    }

    if (S_ISREG(st.st_mode)) {
        // Quick ceiling q = (x + y - 1) / y;
        if (opt->apparent_size) {
            size = (st.st_size + opt->block_size - 1) / opt->block_size;
        }
        else {
            size = (((st.st_size + st.st_blksize - 1) / st.st_blksize)
                + opt->block_size - 1) / opt->block_size;
        }
        if (opt->all) {
            //prints the size information according to the options
            print_file(size, completePath);   
        }
    }
    else if (S_ISDIR(st.st_mode) && strcmp(name, ".") && strcmp(name, "..")) {
        fprintf(stderr, "Unhandled directory\n");
        fprintf(stderr, completePath);
    }
    else if (S_ISLNK(st.st_mode)) {
        fprintf(stderr, "Unhandled symlink\n");
        fprintf(stderr, completePath);
        return 1;
    }

    return size;
}