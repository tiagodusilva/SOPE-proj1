#include "../include/handleSignal.h"


/**
 * @brief It handles the father SIGINT interrupt
 * 
 */
void handlerFather(int signo){
    char key[1]; 
    //STOP children
    writeInLog(RECV_SIGNAL, "SIGINT");
    killpg(thisOpt->child_pgid, SIGSTOP);
    
    write(STDOUT_FILENO, "|| [Y/y] to proceed | any other key to continue ||", 50); 
    read(STDIN_FILENO, &key, 1);

    if (strcmp("Y", key) == 0 || strcmp("y", key) == 0){
        sendSignal(getpid(), "GP SIGINT"); 
        writeInLog(RECV_SIGNAL, "SIGINT"); 
        killpg(thisOpt->child_pgid, SIGINT);
        exit(1);
    }
    
    //CONTINUE children
    sendSignal(getpid(), "GP SIGCONT"); 
    killpg(thisOpt->child_pgid, SIGCONT);

}

void handlerChild_sigCont(int signo){
    writeInLog(RECV_SIGNAL, "SIGCONT"); 
}

void handlerChild_sigInt(int signo){
    writeInLog(RECV_SIGNAL, "SIGINT"); 
}

int setSignal(Options *opt){
    
    struct sigaction act; 
    thisOpt = opt; 

    if (sigemptyset(&act.sa_mask) < 0){
        perror("On sigempty %d"); 
        return 1; 
    }

    if (opt->original_process) {
        act.sa_handler = handlerFather;
        if (sigaction(SIGINT, &act, NULL) < 0){
            perror("Error set father signal"); 
            return 1; 
        } 
    }
    else{
        act.sa_handler = handlerChild_sigCont; 
        if (sigaction(SIGCONT, &act, NULL) < 0){
            perror("Error set child signal, SIGCONT"); 
            return 1; 

        }

        act.sa_handler= handlerChild_sigInt; 
        if (sigaction(SIGINT, &act, NULL) < 0){
            perror("Error set child singla, SIGINT");
            return 1; 
        }

    }
    return 0;
}

