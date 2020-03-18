#ifndef SOPE_PROJ1_HANDLELOG_H
#define SOPE_PROJ1_HANDLELOG_H

#include <fcntl.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <string.h> 

/** @brief Max size of the name of log file**/ 
#define MAX_SIZE_LOG        100

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

#endif //SOPE_PROJ1_HANDLELOG_H