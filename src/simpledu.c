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


int main(int argc, char *argv[], char *envp[]) {
    Options opt;             
   
    char optString[MAX_SIZE_LINE]; 
    for (int i = 0; i < argc; i++){
        strcat(optString, argv[i]); 
        strcat(optString, " ");
    }  
    writeInLog(0, optString); 

    simpledu_startup(argc, argv, &opt);

    if (showDirec(&opt)){
        perror("Show directory error");
        exit(1); 
    }

    simpledu_shutdown(&opt);

    exit(0);
}