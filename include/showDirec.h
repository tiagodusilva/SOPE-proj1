#ifndef SOPE_PROJ1_SHOWFILE_H
#define SOPE_PROJ1_SHOWFILE_H

#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include "../include/utils.h"
#include "../include/handleLog.h"

    
#define PATH_SIZE_MAX       540
#define PATH_MAX_CPY        180


/**
 * @brief This function handles the task of showing files information
 * 
 * @param opt Options given as parameters
 * @return int 0 if success and 1 upon error
 */
int showDirec(Options *opt);

/**
 * @brief For each file received, it prints the file information i.e size.  
 * 
 * @param opt Options given as parameters
 * @param name Name of the file in the folder
 * @return lont int The file's size upon success, -1 otherwise
 */
long int analyze_file(Options* opt, char *name);

#endif //SOPE_PROJ1_SHOWFILE_H
