#include "../include/utils.h"

int is_num(char *s) {
    while (*s != '\0') {
        if (*s < '0' || *s > '9')
            return 1;
        ++s;
    }
    return 0;
}