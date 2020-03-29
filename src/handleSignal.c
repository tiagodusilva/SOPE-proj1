#include "../include/handleSignal.h"


/**
 * @brief It handles the father SIGINT interrupt
 * 
 */
void handlerFather(int signo){
    char key[1]; 
    //STOP children
    for (int i = 0; i < thisOpt->sizeChildProcess; i++){
        writeInLog(SEND_SIGNAL, "FATHER SEND STOP"); 
        killpg(thisOpt->childProcess[i], SIGSTOP);        
    }
        
    write(STDOUT_FILENO, "Y/y to proceed | any other key to continue", 42); 
    read(STDIN_FILENO, &key, 1);

    if (strcmp("Y", key) == 0 || strcmp("y", key) == 0){
        signal(SIGINT, SIG_DFL); 
        raise(SIGINT); 
    }
    
    //CONTINUE children
    for (int i = 0; i < thisOpt->sizeChildProcess; i++){
        writeInLog(SEND_SIGNAL, "FATHER SEND CONTINUE"); 
        killpg(thisOpt->childProcess[i], SIGCONT);       
    } 

}

int setSignal(Options *opt){
    struct sigaction act; 
    thisOpt = opt; 

    if (sigemptyset(&act.sa_mask) < 0){
        perror("On sigempty %d"); 
        return 1; 
    }

    if (opt->original_process){
        writeInLog(CREATE, "FATHER");
        act.sa_handler = handlerFather;
        if (sigaction(SIGINT, &act, NULL) < 0){
            perror("Error set father signal"); 
            return 1; 
        } 
    }
    return 0;
}
