#ifndef SOPE_PROJ1_HANDLELOG_H
#define SOPE_PROJ1_HANDLELOG_H

#include <fcntl.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <string.h> 
#define MAX_SIZE_LOG        100

int createLog(char * logName); 
void getLogName(char * logName, char* logFile);

#endif //SOPE_PROJ1_HANDLELOG_H