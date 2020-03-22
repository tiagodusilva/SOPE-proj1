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

    char * father; 
    pid_t pid = getpid();
    char pid_string[20];  
    sprintf(pid_string, "%d", pid); 

    if ((father = getenv("FATHER")) == NULL){           //father creates a new env variable with the pin as value
        if (putenv("FATHER") < 0){
            fprintf(stderr, "Not possible to create FATHER ENV\n");
            exit(1); 
        }
        if (setenv("FATHER", pid_string, 1) < 0){
            fprintf(stderr, "Not possible to set FATHER ENV\n"); 
            exit(1); 
        } 
        father = pid_string; 
    }

    if (strcmp(pid_string, father) == 0){               //if actual pin equals to the father pin, then creates file
        char *logName = getenv("LOG_FILENAME");
        if (createLog(logName)){
            fprintf(stderr, "Error in createLog\n"); 
            exit(1);  
        }
    }

    //testing call
    //writeInLog(17, CREATE, "picuinha"); 


    if (parse_arguments(argc, argv, &opt)) {
        fprintf(stderr, "Invalid command\n");
        exit(1);
    }

    // print_options(&opt);

    if (showDirec(&opt, envp)){
        fprintf(stderr, "Show directory error\n");
        exit(1); 
    }

    if (strcmp(pid_string, father) == 0)                //delete father env variable
        if (unsetenv("FATHER") < 0){
            fprintf(stderr, "Not possible to remove FATHER ENV\n");
            exit(1); 
        }


    close(fd);
    exit(0);
}