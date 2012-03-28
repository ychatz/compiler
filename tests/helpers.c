#include "helpers.h"

char* strreplace(char *str, char old, char new) {
    int i;
    for (i=0; str[i]; ++i) {
        if ( str[i] == old ) str[i] = new;
    }

    return str;
}
