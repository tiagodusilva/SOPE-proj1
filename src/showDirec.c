#include "../include/showDirec.h"
#include <sys/wait.h>
#include <unistd.h>
#include "../include/queue.h"

extern int childProcess[MAX_SIZE_LINE];
extern int sizeChildProcess; 
extern bool isFather;

#define STAT_BLOCK_SIZE 512

#define MAX_STRUCT_NAME     512

typedef struct fileInfo {
    long int file_size;
    bool is_dir, sub_dir_size;
    char name[MAX_STRUCT_NAME];
} FileInfo;

static void read_fileInfo(FileInfo *fi, int fileno) {
    read(fileno, fi, sizeof(*fi));
}

static void write_fileInfo(FileInfo *fi, int fileno) {
    write(fileno, fi, sizeof(*fi));
}

// Calculate only before printing
static inline long int calculate_size(long int size, Options *opt) {
        // Quick ceiling q = (x + y - 1) / y;
        return (size + opt->block_size - 1) / opt->block_size;
}

// For safekeeping :D
static inline long int get_size(struct stat *st, Options *opt) {
    return opt->apparent_size ?
        st->st_size
        : st->st_blocks * STAT_BLOCK_SIZE;
}

static inline void print_fileInfo(FileInfo *fi, Options *opt) {
    char number[24];
    sprintf(number, "%ld", calculate_size(fi->file_size, opt));
    
    for (int i = strlen(number) % 8; i < 8; ++i)
        strcat(number, " ");

    write(STDOUT_FILENO, number, strlen(number));
    write(STDOUT_FILENO, fi->name, strlen(fi->name));
    write(STDOUT_FILENO, "\n", 1);
}

static inline void handle_file_output(FileInfo *fi, Options *opt) {
    if (opt->all) {
        if (!opt->max_depth || opt->depth_val > 0) {
            if (isFather)
                print_fileInfo(fi, opt);
            else
                write_fileInfo(fi, STDOUT_FILENO);
        }
    }
}

static inline void handle_dir_output(FileInfo *fi, Options *opt) {
    // This never handles the case of printing the original
    // processe's directory at the end of everything
    if (isFather)
        if (!opt->max_depth || opt->depth_val > 0)
            print_fileInfo(fi, opt);
    else {
        if (!opt->separate_dirs)
            write_fileInfo(fi, STDOUT_FILENO);
    }
}

/**
 * @brief For each file received, it prints the file information i.e size.  
 * 
 * @param opt Options given as parameters
 * @param name Name of the file in the folder
 * @return lont int The file's size upon success, -1 otherwise
 */
static long int analyze_file(Options* opt, char *name, Queue_t *queue){
    struct stat st; 
    
    FileInfo fi;
    fi.file_size = 0;
    fi.is_dir = false;
    fi.sub_dir_size = false;
    fi.name[0] = '\0';

    //get the complete path of the file called "name"
    strncpy(fi.name, opt->path, MAX_STRUCT_NAME);
    if (fi.name[strlen(fi.name) - 1] != '/')
        strcat(fi.name, "/");
    strncat(fi.name, name, MAX_STRUCT_NAME);

    if (lstat(fi.name, &st) < 0){
        perror("Error getting the file's stat struct");
        exit(-1); 
    }

    if (S_ISLNK(st.st_mode) && !opt->dereference) {
        // Don't follow symbolic link
        // If not flag apparent size, assume size is 0
        if (opt->apparent_size)
            fi.file_size = get_size(&st, opt);
        else // TODO: Test without this if statement
            fi.file_size = 0;
        handle_file_output(&fi, opt);
        return fi.file_size;
    }
    else {
        if (S_ISLNK(st.st_mode)) {
            // If it was a symlink and wee have to dereference it
            // We stat the file it points to instead
            if (stat(fi.name, &st) < 0){
                perror("Error getting the file's stat struct");
                exit(-1); 
            }
        }

        if (S_ISREG(st.st_mode)) {
            fi.file_size = get_size(&st, opt);
            handle_file_output(&fi, opt);
            return fi.file_size;
        }
        else if (S_ISDIR(st.st_mode) && strcmp(name, "..")) {
            if (strcmp(name, ".")) {
                // If it's not a '.' file
                fi.is_dir = true;
                if (opt->separate_dirs && (!opt->max_depth || opt->depth_val > 0)) {
                    // ADD TO QUEUE
                }
                return 0;
            }
            else {
                // It's the directory itself ('.')
                fi.file_size = get_size(&st, opt);
                return fi.file_size;
            }
        }
    }

    return 0;
}

int showDirec(Options * opt) {
    DIR * direc;
    struct dirent * dirent;
    long int tmp;

    FileInfo cur_dir;
    if (!opt->separate_dirs)
        cur_dir.sub_dir_size = true;
    cur_dir.file_size = 0;
    strncpy(cur_dir.name, opt->path, MAX_STRUCT_NAME);

    Queue_t *queue = new_queue();
    if (queue == NULL) {
        perror("Failed to instanteate the queue");
        exit(1);
    }

    if ((direc = opendir(opt->path)) == NULL){  
        perror("Not possible to open directory");
        exit(1);
    }

    // Take care of all regular files and save our directories for later
    while((dirent = readdir(direc)) != NULL){
        if (tmp = analyze_file(opt, dirent->d_name, queue), tmp == -1)  
            return 1;
        cur_dir.file_size += tmp;
    }

    if (closedir(direc) == -1){
        perror("Not possible to close directory\n");
        return 1;
    }

    if ((!opt->separate_dirs && (!opt->max_depth || opt->depth_val <= 0)) && !queue_is_empty(queue)) {
        
        // There are more subdirectories to take care of
        int pipe_id[2];
    
        // Create pipe
        if (pipe(pipe_id)) {
            perror("Failed to create pipe");
            exit(1);
        }

        while (!queue_is_empty(queue)) {
            int id = fork();
            if (id) {
                // Father
                childProcess[sizeChildProcess++] = id;

                close(pipe_id[PIPE_WRITE]);
                free(queue_pop(queue));
            }
            else {
                // Child
                if (isFather) {
                    int newgrp;
                    if ((newgrp = setpgrp()) < 0) {
                        perror("Error on setprgrp\n");
                        exit(1);
                    }
                }

                // Handles dup2 with stdout and pipe
                dup2(pipe_id[PIPE_WRITE], STDOUT_FILENO);

                close(pipe_id[PIPE_READ]);
                exec_next_dir(queue_front(queue), opt);
                perror("Failed to exec to the next sub directory");
                queue_clear(queue);
                exit(1);
            }
        }

        // Time to wait and read from ALL my children :<)
        int aux = -1;

    }

    if (isFather)
        print_fileInfo(&cur_dir, opt);
    else
        handle_dir_output(&cur_dir, opt);

    return 0;
}
