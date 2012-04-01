#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "tokens.h"

extern char *yytext;
extern FILE *yyin;
extern int lineno;
extern int errno;

int ERROR (const char msg []) {
    fprintf(stderr, "ERROR, line %d: %s\n", lineno, msg);
    exit(1);
}

int main (int argc, char **argv) {
    int token;

    if ( argc == 2 ) {
        yyin = fopen(argv[1], "r");

        if (yyin == NULL) {
            fprintf(stderr, "ERROR, could not open \"%s\": %s\n", argv[1], strerror(errno));
            exit(1);
        }
    }
    else {
        yyin = stdin;
    }

    do {
        token = yylex();
        printf("token=%d, lexeme=\"%s\"\n", token, yytext);
    } while (token != T_eof);

    return 0;
}
