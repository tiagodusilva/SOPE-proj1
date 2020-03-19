#include "../include/showDirec.h"

static inline void print_file(long int size, char *s) {
    printf("%-8ld%s\n", size, s);
}

static inline long int calculate_size(struct stat *st, Options *opt) {
        // Quick ceiling q = (x + y - 1) / y;
        if (opt->apparent_size) {
            return (st->st_size + opt->block_size - 1) / opt->block_size;
        }
        else {
            return (((st->st_size + st->st_blksize - 1) / st->st_blksize) * st->st_blksize
                + opt->block_size - 1) / opt->block_size;
        }
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
    long int size = 0; 

    //get the complete path of the file called "name"
    char completePath[PATH_SIZE_MAX] = ""; 
    strcpy(completePath, opt->path);
    if (completePath[strlen(completePath) - 1] != '/')
        strcat(completePath, "/");
    strcat(completePath, name);

    if (lstat(completePath, &st) < 0){
        fprintf(stderr, "Not possible to get file stat\n"); 
        return -1; 
    }

    if (S_ISREG(st.st_mode)) {
        size = calculate_size(&st, opt);
        if (opt->all) {
            //prints the size information according to the options
            print_file(size, completePath);   
        }
    }
    else if (S_ISDIR(st.st_mode) && strcmp(name, "..")) {
        if (strcmp(name, ".")) {
            // If it's not a '.' file
            fprintf(stderr, "Unhandled directory\n");
            fprintf(stderr, "%s\n", completePath);
        }
        else {
            size = calculate_size(&st, opt);
        }
    }
    else if (S_ISLNK(st.st_mode) && opt->dereference) {
        /*
        if (stat(completePath, &st) < 0){
            fprintf(stderr, "Not possible to get file stat\n"); 
            return -1; 
        }
        //Recursion?
        */
        fprintf(stderr, "Unhandled symlink - Follow\n");
        fprintf(stderr, "%s\n", completePath);
        return 1;
    }
    else if (S_ISLNK(st.st_mode)) {
        size = calculate_size(&st, opt);
        if (opt->all) {
            //prints the size information according to the options
            print_file(size, completePath);   
        }
    }

    return size;
}