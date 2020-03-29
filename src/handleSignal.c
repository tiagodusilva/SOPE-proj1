#include "../include/handleSignal.h"


/**
 * @brief It handles the father SIGINT interrupt
 * 
 */
void handlerFather(int signo){
    char key[1]; 
    //STOP children
    
    for (int i = 0; i < thisOpt->sizeChildProcess; i++){
        killpg(thisOpt->childProcess[i], SIGUSR2);        
    }
        
    write(STDOUT_FILENO, "Y/y to proceed | any other key to continue", 42); 
    read(STDIN_FILENO, &key, 1);

    if (strcmp("Y", key) == 0 || strcmp("y", key) == 0){
        for  (int i = 0; i < thisOpt->sizeChildProcess; i++)
            killpg(thisOpt->childProcess[i], SIGUSR1);
        sendSignal(getpid(), "SIGINT"); 
        signal(SIGINT, SIG_DFL); 
        raise(SIGINT); 
    }
    
    //CONTINUE children
    writeInLog(SEND_SIGNAL, "SIGCONT"); 
    for (int i = 0; i < thisOpt->sizeChildProcess; i++){
        writeInLog(SEND_SIGNAL, "GP SIGCONT"); 
        killpg(thisOpt->childProcess[i], SIGCONT);       
    } 

}

void handlerChild_sigint(int signo){
    writeInLog(RECV_SIGNAL, "SIGINT");
    raise(SIGINT); 
}

void handlerChild_sigstop(int signo){
    writeInLog(RECV_SIGNAL, "SIG_STOP"); 
    raise(SIGSTOP); 
}


int setSignal(Options *opt){
    struct sigaction act; 
    thisOpt = opt; 

    if (sigemptyset(&act.sa_mask) < 0){
        perror("On sigempty %d"); 
        return 1; 
    }


    if (opt->original_process){
        act.sa_handler = handlerFather;
        if (sigaction(SIGINT, &act, NULL) < 0){
            perror("Error set father signal"); 
            return 1; 
        } 
    }else{
        act.sa_handler = handlerChild_sigint; 
        if (sigaction(SIGUSR1, &act, NULL) < 0){
            perror("Error set father signal"); 
            return 1; 
        } 
        act.sa_handler= handlerChild_sigstop; 
        if (sigaction(SIGUSR2, &act, NULL) < 0){
            perror("Error set father signal"); 
            return 1; 
        } 
    }
    return 0;
}

