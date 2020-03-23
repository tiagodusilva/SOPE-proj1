#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "../include/utils.h"
#include "../include/showDirec.h"
#include "../include/handleLog.h"


int fd; /** @brief File descriptor for the log file**/  

int childProcess[MAX_SIZE_LINE];
int sizeChildProcess; 

void handlerFather(int signo){
    printf("Interrupt process? [Y|N]\n");
    writeInLog(42, RECV_SIGNAL, "FATHER");
    if (!askEnd()) return; 
    int i; 
    for (i = 0; i < sizeChildProcess; i++) {
        writeInLog(43, SEND_SIGNAL, "FATHER"); 
        kill(childProcess[i], SIGUSR1); 
    }
    signal(SIGINT, SIG_DFL);
    raise(SIGINT); 
}
void handlerChild(int signo){
    int i; 
    writeInLog(44, RECV_SIGNAL, "CHILD");
    for (i = 0; i < sizeChildProcess; i++){
        writeInLog(43, SEND_SIGNAL, "CHILD"); 
        kill(childProcess[i], SIGUSR1); 
    }
    raise(SIGINT); 
}

int main(int argc, char *argv[], char *envp[]) {
    Options opt;
    sizeChildProcess = 0;                              
    pid_t pid = getpid();
    char pid_string[20];  
    char *logName = getenv("LOG_FILENAME");
    sprintf(pid_string, "%d", pid); 
    bool isFather = false; 
    struct sigaction act; 

    
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

    //Handle signal -------------------------------
    if (sigemptyset(&act.sa_mask) < 0){
        fprintf(stderr, "Error on sigemptyset\n");
        exit(1); 
    }
    if (isFather){
        writeInLog(17, CREATE, "FATHER");
        act.sa_handler = handlerFather;
        if (sigaction(SIGINT, &act, NULL) < 0){
            fprintf(stderr, "Error on father sigaction\n"); 
            exit(1); 
        } 
    }
    else{
        writeInLog(17, CREATE, "CHILD");
        act.sa_handler = handlerChild; 
        if (sigaction(SIGUSR1, &act, NULL) < 0){
            fprintf(stderr, "Error on child sigaction\n");
            exit(1); 
        }
    }


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

    //sleep(1);
    if (isFather) {
        writeInLog(10, EXIT, "FATHER");
        close(fd);
    }
    else writeInLog(10, EXIT, "CHILD");
    sleep(1);
    exit(0);
}