#include "../include/handleLog.h"

int createLog(char * logName){
    char logfile[MAX_SIZE_LOG];
    getLogName(logName, logfile); 
     
 
    if ((fd = open(logfile, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC, S_IRUSR | S_IWUSR)) < 0){
        fprintf(stderr, "Not possible to open file %s\n", logfile); 
        return 1; 
    }
    
    return 0;
} 


void getLogName(char * logName, char* logFile){

    if (!logName) strcpy(logFile, "output");        //default file
    else strncpy(logFile, logName, MAX_SIZE_LOG);   //if given by user
         
}

int writeInLog(double instant, action a, char *info){
    pid_t pid = getpid(); 
    char line [MAX_SIZE_LINE] = {0};  
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

    
    snprintf(line, MAX_SIZE_INFO, "%-8.2f - %-8d - %-15s - %s \n", instant, pid, action, info);
    
    if (write(fd, line, MAX_SIZE_INFO) == -1){
        fprintf(stderr, "Impossible to write\n"); 
        return 1; 
    }
    return 0; 
}