#include "../include/showDirec.h"
#include <sys/wait.h>
#include <unistd.h>

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

int showDirec(Options * opt) {
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
    //printf("%d\n", getpid());

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
            // fprintf(stderr, "Unhandled directory\n");
            // fprintf(stderr, "%s\n", completePath);

            // Create pipe
            int pipe_id[2];
            if (pipe(pipe_id)) {
                fprintf(stderr, "Failed to create pipe\n");
                exit(1);
            }

            // Handles dup2 with stdout and pipe
            int original_stdout = dup(STDOUT_FILENO);
            dup2(pipe_id[PIPE_WRITE], STDOUT_FILENO);

            // Fork process
            int id = fork();
            if (id) {
                // Parent
                close(pipe_id[PIPE_WRITE]);
                dup2(original_stdout, STDOUT_FILENO);

                char *line = calloc(MAXLINE, sizeof(*line));
                int read_ret, status, wait_ret;
                while (wait_ret = waitpid(id, &status, WNOHANG), wait_ret == 0) {
                    while (read_ret = read(pipe_id[PIPE_READ], line, MAXLINE), read_ret) {
                        if (read_ret == -1) {
                            fprintf(stderr, "Error while reading form the child's pipe\n");
                            exit(1);
                        }
                        printf(line);
                    }
                }
                if (wait_ret == -1) {
                    fprintf(stderr, "Error in waitpid()\n");
                }
                close(pipe_id[PIPE_READ]);

                if (!opt->separate_dirs) {
                    size = atoi(line);
                }

            }
            else {
                // Child
                close(pipe_id[PIPE_READ]);
                exec_next_dir(completePath, opt);
                fprintf(stderr, "Failed to exec the folder '%s'", completePath);
                exit(1);
            }


        }
        else {
            size = calculate_size(&st, opt);
        }
    }
    else if (S_ISLNK(st.st_mode)) {
        
        if (opt->dereference) {
            // Follow symbolic link
            
            struct stat link_st;

            if (stat(completePath, &link_st) < 0){
                fprintf(stderr, "Not possible to get symbolic link's file stat\n"); 
                return -1;
            }

            if (S_ISREG(link_st.st_mode)) {
                size = calculate_size(&link_st, opt);
                if (opt->all) {
                    //prints the size information according to the options
                    print_file(size, completePath);   
                }
            }
            else if (S_ISDIR(link_st.st_mode)) {
                fprintf(stderr, "Unhandled symlink directory - Follow\n");
                fprintf(stderr, "%s\n", completePath);
            }

        }
        else {
            // Don't follow symbolic link
            if (opt->all) {
                //prints the size information according to the options
                if (opt->apparent_size)
                    size = calculate_size(&st, opt);
                // If not flag apparent size, assume size is 0, which is it's default value
                print_file(size, completePath);
            }
        }
    }

    return size;
}
