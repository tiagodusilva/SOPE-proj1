#include "../include/handleLog.h"

int createLog(char * logName){
    char logfile[MAX_SIZE_LOG];
    getLogName(logName, logfile); 
    pid_t pid; 
 
    if ((pid = open(logfile, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC)) < 0){
        fprintf(stderr, "Not possible to open file %s", logfile); 
        return -1; 
    }
    return pid; 
} 


void getLogName(char * logName, char* logFile){
    
    if (!logName) strcpy(logFile, "output");
    else strncpy(logFile, logName, MAX_SIZE_LOG); 
         
    
}