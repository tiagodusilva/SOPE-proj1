#ifndef SOPE_PROJ1_SHOWFILE_H
#define SOPE_PROJ1_SHOWFILE_H

#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include "../include/utils.h"


#define PATH_SIZE_MAX       500

int showDirec(Options *opt);
int printFileState(Options* opt, char *name);

#endif //SOPE_PROJ1_SHOWFILE_H
