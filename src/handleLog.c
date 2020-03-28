#include "../include/handleLog.h"

#define NO_LOGS -42

static int log_fd; /** @brief File descriptor for the log file**/ 

/**
 * @brief Creates logFile name 
 * 
 * @param logName Name of the variable LOG_FILENAME
 * @return int -1 upon error, else the pid of the created file
 */
static int createLog(char * logName){     
 
    if ((log_fd = open(logName, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND | O_SYNC, S_IRUSR | S_IWUSR)) < 0){
        fprintf(stderr, "Not possible to open file %s\n", logName); 
        return 1; 
    }
    
    return 0;
}


/**
 * @brief It opens the log file in append mode
 * 
 * @param logName Name of the log file
 * @return int 0 upon success, 1 otherwise
 */
static int openLog(char * logName){
 
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

int writeInLog(double instant, action a, char *info){

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
            strncpy(action, "CREATE", MAX_SIZE_ACTION);
            break;
        }

        int sizeWritten = snprintf(line, MAX_SIZE_INFO, "%-8.2f - %-8d - %-15s - %s \n", instant, pid, action, info);
        lseek(log_fd, +0, SEEK_END); 
        if (write(log_fd, line, sizeWritten) == -1){
            fprintf(stderr, "Impossible to write on folder\n"); 
            return 1; 
        }
    }

    return 0; 
}

void closeLog(Options *opt) {

    if (log_fd != NO_LOGS) {
        if (opt->original_process) {
            writeInLog(10, EXIT, "FATHER");
            close(log_fd);
        }
        else writeInLog(10, EXIT, "CHILD");
    }

}
