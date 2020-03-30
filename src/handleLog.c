#include "../include/handleLog.h"

#include <sys/time.h>

#define NO_LOGS -42

#define START_TIME_FILENAME "/tmp/simpledu_start_time"

static int log_fd = NO_LOGS; /** @brief File descriptor for the log file**/ 
static struct timeval start;

static inline void fileInfoString(FileInfo *fi, char* res){
    sprintf(res, " %ld %s || IS_DIR: %d || IS_SUBDIR: %d", fi->file_size, fi->name, fi->is_dir, fi->is_sub_dir); 
}

/**
 * @brief Creates logFile name 
 * 
 * @param logName Name of the variable LOG_FILENAME
 * @return int -1 upon error, else the pid of the created file
 */
static int createLog(char * logName){
    // Only called by father, we can exploit that to set the initial instant

    gettimeofday(&start, NULL);
    int start_fd;
    if ((start_fd = open(START_TIME_FILENAME, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC, S_IRUSR | S_IWUSR)) < 0) {
        perror("Failed to create the tmp file");
        return 1;
    }
    if (write(start_fd, &start, sizeof(struct timeval)) < 0) {
        perror("Failed to write the initial instant to the tmp file");
        return 1;
    }
    close(start_fd);

    if ((log_fd = open(logName, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND | O_SYNC, S_IRUSR | S_IWUSR)) < 0){
        fprintf(stderr, "Not possible to open file %s\n", logName); 
        return 1; 
    }
    
    return 0;
}

static inline double get_instant() {
    struct timeval end;
    gettimeofday(&end, NULL);
    return ((end.tv_sec - start.tv_sec) * 1000000u +
            end.tv_usec - start.tv_usec) / 1.e6;
}

/**
 * @brief It opens the log file in append mode
 * 
 * @param logName Name of the log file
 * @return int 0 upon success, 1 otherwise
 */
static int openLog(char * logName) {
    // Only called by children, we can exploit that :D

    int start_fd;
    if ((start_fd = open(START_TIME_FILENAME, O_RDONLY, S_IRUSR | S_IWUSR)) < 0) {
        perror("Failed to open the tmp file");
        return 1;
    }
    if (read(start_fd, &start, sizeof(struct timeval)) < 0) {
        perror("Failed to read the initial instant to the tmp file");
        return 1;
    }
    close(start_fd);
    

    if ((log_fd = open(logName, O_WRONLY | O_SYNC | O_APPEND, S_IRUSR | S_IWUSR)) < 0){
        fprintf(stderr, "Not possible to open file %s\n", logName); 
        return 1; 
    }
    
    return 0;
}

void startLog(Options *opt) {

    char *logName = getenv("LOG_FILENAME");
    log_fd = NO_LOGS;

    if (logName != NULL) {
        if (opt->original_process){                                              //If actual pin equals to the father pin, then creates file
            if (createLog(logName)){
                fprintf(stderr, "Error in createLog\n"); 
                exit(1);  
            }
        }
        else{
            if(openLog(logName)){
                fprintf(stderr, "Error opening log file\n");
                exit(1);
            }
        }
    }

}

int writeInLog(action a, char *info){

    if (log_fd != NO_LOGS) {
        pid_t pid = getpid(); 

        char line [MAX_SIZE_LINE] = "";  
        char action[MAX_SIZE_ACTION];
        //handle the enum
        switch (a)
        {
        case CREATE: 
            strncpy(action, "CREATE", MAX_SIZE_ACTION); 
            break;
        case EXIT: 
            strncpy(action, "EXIT", MAX_SIZE_ACTION);
            break; 
        case SEND_SIGNAL: 
            strncpy(action, "SEND_SIGNAL", MAX_SIZE_ACTION);
            break; 
        case RECV_SIGNAL: 
            strncpy(action, "RECV_SIGNAL", MAX_SIZE_ACTION);
            break; 
        case RECV_PIPE: 
            strncpy(action, "RECV_PIPE", MAX_SIZE_ACTION);
            break; 
        case SEND_PIPE: 
            strncpy(action, "SEND_PIPE", MAX_SIZE_ACTION);
            break; 
        case ENTRY:
            strncpy(action, "ENTRY", MAX_SIZE_ACTION);
            break; 
        default:
            strncpy(action, "UNKNOWN", MAX_SIZE_ACTION);
            break;
        }

        int sizeWritten = snprintf(line, MAX_SIZE_INFO, "%-8.2f - %-8d - %-15s - %s \n", get_instant(), pid, action, info);
    
        if (write(log_fd, line, sizeWritten) == -1){
            perror("Failed to write to log");
            return 1;
        }
    }

    return 0; 
}

void closeLog(Options *opt) {

    if (log_fd != NO_LOGS) {
        if (opt->original_process) {
            writeInLog(EXIT, "FATHER");
            close(log_fd);
            remove(START_TIME_FILENAME);
        }
        else {
            writeInLog(EXIT, "CHILD");
            close(log_fd);
        }
    }

}

 void info_pipe(FileInfo *fi, action a){
    char *aux = (char*)calloc(MAX_SIZE_LINE, sizeof(char));

    fileInfoString(fi, aux);     
    writeInLog(a, aux); 
    free(aux); 
 }


void entry(FileInfo cur_dir, Options *opt){
    char c[MAX_SIZE_INFO]; 
    sprintf(c, "%ld %s", cur_dir.file_size, opt->path); 
    writeInLog(ENTRY, c); 
}

void sendSignal(pid_t pid, char * signal){
    char aux[MAX_SIZE_LINE];
    sprintf(aux, "%s %d", signal, pid); 
    writeInLog(SEND_SIGNAL, aux); 
}

void create(int argc, char* argv[]){
    
    char *optString = (char * )calloc(MAX_PATH_SIZE, sizeof(char)); 
    for (int i = 0; i < argc; i++){
        strcat(optString, argv[i]); 
        strcat(optString, " ");
    }
    writeInLog(CREATE, optString); 
    free(optString);
}