#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "../include/utils.h"
#include "../include/showDirec.h"
#include "../include/handleLog.h"


int fd; /** @brief File descriptor for the log file**/  

int main(int argc, char *argv[], char *envp[]) {
    Options opt;
                                  
    pid_t pid = getpid();
    char pid_string[20];  
    char *logName = getenv("LOG_FILENAME");
    sprintf(pid_string, "%d", pid); 
    bool isFather = false; 


    if (getenv("SIMPLEDUFATHER") == NULL){                    //Father creates a new env variable with it's pin as value
        isFather = true; 
        if (putenv("SIMPLEDUFATHER") < 0){
            fprintf(stderr, "Not possible to create FATHER ENV\n");
            exit(1); 
        }
        if (setenv("SIMPLEDUFATHER", pid_string, 1) < 0){
            fprintf(stderr, "Not possible to set FATHER ENV\n"); 
            exit(1); 
        } 
    }

    if (isFather){                        //If actual pin equals to the father pin, then creates file
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

    //testing call
    writeInLog(17, CREATE, "NONE"); 


    if (parse_arguments(argc, argv, &opt)) {
        fprintf(stderr, "Invalid command\n");
        exit(1);
    }

    // print_options(&opt);

    if (showDirec(&opt)){
        fprintf(stderr, "Show directory error\n");
        exit(1); 
    }

    if (isFather){               //Delete father env variable
        if (unsetenv("SIMPLEDUFATHER") < 0){
            fprintf(stderr, "Not possible to remove FATHER ENV\n");
            exit(1); 
        }
    }

    close(fd);
    exit(0);
}