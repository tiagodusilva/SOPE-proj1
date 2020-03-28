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

/** @brief Max number of childs */
#define MAX_CHILDREN 100

int childProcess[MAX_CHILDREN];    /** @brief Pid of direct children */
int sizeChildProcess = 0;               /** @brief size of childProcess vector*/

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
    
    //CONTINUE children
    for (int i = 0; i < sizeChildProcess; i++){
        writeInLog(42, SEND_SIGNAL, "FATHER SEND CONTINUE"); 
        killpg(childProcess[i], SIGCONT);       
    } 

}

int main(int argc, char *argv[], char *envp[]) {
    Options opt;               

    simpledu_startup(argc, argv, &opt);

    //Handle signal -------------------------------
    struct sigaction act;
    if (sigemptyset(&act.sa_mask) < 0){
        fprintf(stderr, "Error on sigemptyset\n");
        exit(1); 
    }
    if (opt.original_process){
        writeInLog(17, CREATE, "FATHER");
        act.sa_handler = handlerFather;
        if (sigaction(SIGINT, &act, NULL) < 0){
            fprintf(stderr, "Error on father sigaction\n"); 
            exit(1); 
        } 
    }

    //---------------------------------------------

    if (showDirec(&opt)){
        perror("Show directory error");
        exit(1); 
    }

    simpledu_shutdown(&opt);

    exit(0);
}