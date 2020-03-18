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

    
    char *logName = getenv("LOG_FILENAME");
    if (createLog(logName)){
        fprintf(stderr, "Error in createLog\n"); 
        exit(1); 
    } 
    char h[7] = "house"; 
    writeInLog(14.4545, CREATE, h);

    close(fd);

    if (parse_arguments(argc, argv, &opt)) {
        fprintf(stderr, "Invalid command\n");
        exit(1);
    }

    // print_options(&opt);

    if (showDirec(&opt)){
        fprintf(stderr, "Show directory error\n");
        exit(1); 
    }
    
    exit(0);
}