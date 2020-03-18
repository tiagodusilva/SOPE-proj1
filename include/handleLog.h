#ifndef SOPE_PROJ1_HANDLELOG_H
#define SOPE_PROJ1_HANDLELOG_H

#include <fcntl.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <string.h> 
#include <unistd.h> 

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

extern int fd;

/** @brief Max size of the name of log file**/ 
#define MAX_SIZE_LOG        100
/** @brief Max size for the message to be written in one line of log file**/
#define MAX_SIZE_LINE       800

#define MAX_SIZE_INFO       700

#define MAX_SIZE_ACTION     20
/**
 * @brief Creates logFile name 
 * 
 * @param logName Name of the variable LOG_FILENAME
 * @return int -1 upon error, else the pid of the created file
 */
int createLog(char * logName); 

/**
 * @brief Get the name of the actual the log file
 * 
 * @param logName Name of the LOG_FILENAME env
 * @param logFile Name of the actual log file to be set by the function 
 */
void getLogName(char * logName, char* logFile);

/**
 * @brief Function that writes in the LOG folder
 * 
 * @param instant The time the information is being written
 * @param a Enumerator indicating the action 
 * @param info Extra information
 * @return int 
 */
int writeInLog(double instant, action a, char * info); 

#endif //SOPE_PROJ1_HANDLELOG_H
