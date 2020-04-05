#pragma once

#include <fcntl.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <string.h> 
#include <unistd.h> 

#include "../include/utils.h"


/**
 * @brief Enum for action to written, in order to avoid string errors
 * 
 */
typedef enum{
    CREATE, 
    EXIT, 
    RECV_SIGNAL, 
    SEND_SIGNAL, 
    RECV_PIPE, 
    SEND_PIPE, 
    ENTRY
}action; 

/** @brief Max size of the name of log file**/ 
#define MAX_SIZE_LOG        100
/** @brief Max size for the message to be written in one line of log file**/
#define MAX_SIZE_LINE       800

#define MAX_SIZE_INFO       700

#define MAX_SIZE_ACTION     20



/**
 * @brief Starts the log file (if necessary)
 * 
 * @param opt 
 */
void startLog(Options *opt);


// int writeInLog(action a, char * info); 

/**
 * @brief Closes the log's file descriptor
 * 
 * @param opt 
 */
void closeLog(Options *opt);
 
  
void log_info_pipe(FileInfo *fi, action a); 

void log_entry(FileInfo *fi, Options *opt);

void log_sendSignal(pid_t pid, char * signal);

void log_receiveSignal(char * signal);

void log_exit(Options *opt);

void log_create(int argc, char* argv[]);
