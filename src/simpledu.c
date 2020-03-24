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

int childProcess[MAX_SIZE_LINE];    /** @brief Pid of direct children */
int sizeChildProcess;               /** @brief size of childProcess vector*/
bool isFather = false;              /** @brief if the process is the main father*/

/**
 * @brief It handles the father SIGINT interrupt
 * 
 */
void handlerFather(int signo){
    //STOP children
    for (int i = 0; i < sizeChildProcess; i++){
        writeInLog(42, SEND_SIGNAL, "FATHER SEND STOP"); 
        killpg(childProcess[i], SIGSTOP);        
    }
        
    write(STDOUT_FILENO, "CTRL-C to proceed | any other to continue", 42); 
    read(STDIN_FILENO, NULL, 1); 
    fflush(STDIN_FILENO); 
    
    //CONTINUE children
    for (int i = 0; i < sizeChildProcess; i++){
        writeInLog(42, SEND_SIGNAL, "FATHER SEND CONTINUE"); 
        killpg(childProcess[i], SIGCONT);       
    } 

}


int main(int argc, char *argv[], char *envp[]) {
    Options opt;                                                /** @brief Parameters of execution */ 
    struct sigaction act;                                        
    char *logName = getenv("LOG_FILENAME");
    pid_t pid = getpid();                                                
    char pid_string[20];  
    sizeChildProcess = 0;                                              

    sprintf(pid_string, "%d", pid); 
    

    //Handle log file -------------------------
    if (getenv("SIMPLEDUFATHER") == NULL){                      //Father creates a new env variable with it's pin as value
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

    if (isFather){                                              //If actual pin equals to the father pin, then creates file
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
    //---------------------------------------------

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

    

    //---------------------------------------------

    if (parse_arguments(argc, argv, &opt)) {
        fprintf(stderr, "Invalid command\n");
        exit(1);
    }

    if (showDirec(&opt)){
        fprintf(stderr, "Show directory error\n");
        exit(1); 
    }

    if (isFather){                                                  //Delete father env variable
        if (unsetenv("SIMPLEDUFATHER") < 0){
            fprintf(stderr, "Not possible to remove FATHER ENV\n");
            exit(1); 
        }
    }
    sleep(1);
    if (isFather) {
        writeInLog(10, EXIT, "FATHER");
        close(fd);
    }
    else writeInLog(10, EXIT, "CHILD");
    exit(0);
}