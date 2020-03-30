#include "../include/showDirec.h"
#include <sys/wait.h>
#include <unistd.h>
#include "../include/queue.h"

#define STAT_BLOCK_SIZE 512


static ssize_t read_fileInfo(FileInfo *fi, int fileno) {
    return read(fileno, fi, sizeof(*fi));
}

static ssize_t write_fileInfo(FileInfo *fi, int fileno) {
    return write(fileno, fi, sizeof(*fi));
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
    printf("%ld\t%s\n", calculate_size(fi->file_size, opt), fi->name);
}

static void handle_file_output(FileInfo *fi, Options *opt) {
    if (opt->all && (!opt->max_depth || opt->depth_val > 0)) {
        if (opt->original_process){
            print_fileInfo(fi, opt);
            info_pipe(fi, RECV_PIPE); 
        }
        else{
            write_fileInfo(fi, STDOUT_FILENO);   
            info_pipe(fi, SEND_PIPE); 
        }     
    }

    
}

// IMPORTANT: Handles only directories read from the pipe
static inline void handle_dir_output(FileInfo *fi, Options *opt) {
    // This never handles the case of printing the original
    // processe's directory at the end of everything
    if (opt->original_process) {
        if (!opt->max_depth || opt->depth_val > 0){
            print_fileInfo(fi, opt);
            info_pipe(fi, RECV_PIPE);
        }
    }
    else {
        // Only case where we want to re-send the directory
        if (!opt->max_depth || opt->depth_val > 0){
            write_fileInfo(fi, STDOUT_FILENO);
            info_pipe(fi, SEND_PIPE);
        }
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
    fi.is_sub_dir = false;
    fi.is_dir = false;
    fi.name[0] = '\0';

    //get the complete path of the file called "name"
    strncpy(fi.name, opt->path, MAX_PATH_SIZE);
    if (fi.name[strlen(fi.name) - 1] != '/')
        strcat(fi.name, "/");
    strncat(fi.name, name, MAX_PATH_SIZE);

    if (lstat(fi.name, &st) < 0){
        perror("Error getting the file's stat struct");
        exit(-1); 
    }

    if (S_ISLNK(st.st_mode) && !opt->dereference) {
        // Don't follow symbolic link
        fi.file_size = get_size(&st, opt);
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
                if (!opt->separate_dirs || !opt->max_depth || opt->depth_val > 0) {
                    // ADD TO QUEUE
                    size_t len = strlen(fi.name) + 1;
                    char *new_path = (char*) malloc(len * sizeof(char));
                    strncpy(new_path, fi.name, len);
                    queue_push_back(queue, new_path);
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
    cur_dir.is_sub_dir = true;
    cur_dir.is_dir = true;
    cur_dir.file_size = 0;
    strncpy(cur_dir.name, opt->path, MAX_PATH_SIZE);

    Queue_t *dir_q = new_queue();
    if (dir_q == NULL) {
        perror("Failed to instanteate the queue");
        exit(1);
    }

    if ((direc = opendir(opt->path)) == NULL){  
        perror("Not possible to open directory");
        exit(1);
    }

    // Take care of all regular files and save our directories for later
    while((dirent = readdir(direc)) != NULL){
        if (tmp = analyze_file(opt, dirent->d_name, dir_q), tmp == -1)  
            return 1;
        cur_dir.file_size += tmp;
    }

        if (closedir(direc) == -1){
            perror("Not possible to close directory\n");
            return 1;
    }
        entry(cur_dir, opt); 

    if (!queue_is_empty(dir_q)) {
        
        Queue_t *pipe_q = new_queue();
        if (pipe_q == NULL) {
            perror("Failed to create the pipe queue");
            exit(1);
        }

        // LAUNCH ALL CHILDS
        while (!queue_is_empty(dir_q)) {
            char *sub_dir = (char*) queue_pop(dir_q);
            
            int *new_pipe = (int*) malloc(sizeof(int[2]));
            if (new_pipe == NULL) {
                perror("Failed to allocate memory for the pipe");
                exit(1);
            }
            queue_push_back(pipe_q, new_pipe);

            if (pipe(new_pipe)) {
                perror("Failed to create pipe");
                exit(1);
            }

            int frk = fork();
            if (frk) {
                // Father
                opt->childProcess[opt->sizeChildProcess++] = frk;
                close(new_pipe[PIPE_WRITE]);
                
                free(sub_dir);
            }
            else {
                // Child
                if (opt->original_process) {
                    int newgrp;
                    if ((newgrp = setpgrp()) < 0) {
                        perror("Error on setprgrp\n");
                        exit(1);
                    }
                }

                // Handles dup2 with stdout and pipe
                if (dup2(new_pipe[PIPE_WRITE], STDOUT_FILENO) == -1) {
                    perror("Failed to dup2");
                    exit(1);
                }
                close(new_pipe[PIPE_READ]);

                // Avoid memory leaks
                // The sub_dir is safe in memory beause it's not in the queue
                free_queue_and_data(dir_q);
                free_queue_and_data(pipe_q);

                // Exec
                exec_next_dir(sub_dir, opt);
                perror("Failed to exec to the next sub directory");
                exit(1);
            }
        }
        
        // TIME TO READ ALL THE CHILD'S PIPES

        int termination_status = 0;
        pid_t any = -1;
        FileInfo received_file;

        while (waitpid(any, &termination_status, WNOHANG) >= 0 || !queue_is_empty(pipe_q)) {

            if (errno != ECHILD && errno != 0) {
                perror("Error on waitpid");
                exit(1);
            }
            
            if (termination_status != 0) {
                fprintf(stderr, "A child has terminated unsuccessfully\n");
                termination_status = 0;
            }

            // Rotate the available pipes
            if (!queue_is_empty(pipe_q)) {
                int *cur_pipe = (int*) queue_pop(pipe_q);
                int read_return = read_fileInfo(&received_file, cur_pipe[PIPE_READ]);

                if (read_return == sizeof(received_file)) {
                    if (received_file.is_sub_dir) {
                        // Last message from that pipe/child
                        if (!opt->separate_dirs)
                            cur_dir.file_size += received_file.file_size;
                        received_file.is_sub_dir = false;

                        // We know it's a directory
                        handle_dir_output(&received_file, opt);

                        close(cur_pipe[PIPE_READ]);
                        free(cur_pipe);
                    }
                    else {
                        // There are still more messages to receive
                        if (received_file.is_dir)
                            handle_dir_output(&received_file, opt);
                        else
                            handle_file_output(&received_file, opt);
                        
                        queue_push_back(pipe_q, cur_pipe);
                    }
                }
                else if (read_return == 0) {
                    queue_push_back(pipe_q, cur_pipe);
                }
                else if (read_return != 0) {
                    perror("Desync caused the read to not have enough bytes in the pipe");
                    free_queue_and_data(pipe_q);
                    free_queue_and_data(dir_q);
                    exit(1);
                }
            }
        }

        free_queue_and_data(pipe_q);
    }

    if (opt->original_process)
        print_fileInfo(&cur_dir, opt);
    else
        write_fileInfo(&cur_dir, STDOUT_FILENO);

    free_queue_and_data(dir_q);
    return 0;
}
