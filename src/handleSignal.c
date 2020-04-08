#include "../include/handleSignal.h"


/**
 * @brief It handles the father SIGINT interrupt
 * 
 */
void handlerFather(int signo) {

    //STOP children
    log_sendSignal(thisOpt->child_pgid, "SIGSTOP");
    killpg(thisOpt->child_pgid, SIGSTOP);

    write(STDERR_FILENO, "\n____ Do you wish to terminate? ('y' to terminate) ____\n", 56);

    char str[MAXLINE+1];
    while (read(STDIN_FILENO, str, MAXLINE) < 0);
    
    if (str[0] == 'y' || str[0] == 'Y') {
        // Terminate children
        log_sendSignal(getpid(), "SIGINT");
        killpg(thisOpt->child_pgid, SIGINT);
        thisOpt->sig_termed_childs = true;
        thisOpt->return_val = 1;
        fprintf(stderr, "I need a medic bag!\n");
        exit(1);
    }
    else {
        // Continue children
        log_sendSignal(getpid(), "SIGCONT"); 
        killpg(thisOpt->child_pgid, SIGCONT);
        raise(SIGCONT);
    }

}

void handlerChild_sigCont(int signo){
    log_receiveSignal("SIGCONT");
}

void handlerChild_sigInt(int signo){
    log_receiveSignal("SIGINT");
    raise(SIGTERM);
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
            perror("Error set father interrupt signal"); 
            return 1; 
        }

    }
    else{
        // act.sa_handler = handlerChild_sigCont; 
        // if (sigaction(SIGCONT, &act, NULL) < 0){
        //     perror("Error set child signal, SIGCONT"); 
        //     return 1; 

        // }

        act.sa_handler= handlerChild_sigInt; 
        if (sigaction(SIGINT, &act, NULL) < 0){
            perror("Error set child singla, SIGINT");
            return 1; 
        }

    }
    return 0;
}

