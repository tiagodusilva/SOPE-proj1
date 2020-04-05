#include "../include/handleSignal.h"

static bool gonnaDieSoon = false;

void alarmHandler(int signo) {
    log_sendSignal(getpid(), "GP SIGCONT"); 
    killpg(thisOpt->child_pgid, SIGCONT);
    gonnaDieSoon = false;
    raise(SIGCONT);
}

/**
 * @brief It handles the father SIGINT interrupt
 * 
 */
void handlerFather(int signo){

    if (!gonnaDieSoon) {
        gonnaDieSoon = true;

        //STOP children
        log_sendSignal(thisOpt->child_pgid, "SIGSTOP");
        killpg(thisOpt->child_pgid, SIGSTOP);

        fflush(stdout);
        write(STDOUT_FILENO, "____ Ctrl+C again to terminate, the program will continue in 2 sec ____", 71);

        alarm(2);
        sleep(2);
    }
    else {
        alarm(0);
        log_sendSignal(getpid(), "GP SIGINT");
        killpg(thisOpt->child_pgid, SIGINT);
        thisOpt->return_val = 1;
        exit(1);
    }

}

void handlerChild_sigCont(int signo){
    log_receiveSignal("SIGCONT");
}

void handlerChild_sigInt(int signo){
    log_receiveSignal("SIGINT");
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

        act.sa_handler = alarmHandler;
        if (sigaction(SIGALRM, &act, NULL) < 0){
            perror("Error set father alarm signal"); 
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

