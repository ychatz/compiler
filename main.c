#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"

extern char *yytext;
extern int lineno;
extern FILE *yyin;

int ERROR (const char msg []) {
    fprintf(stderr, "ERROR, line %d: %s\n",
    lineno, msg);
    exit(1);
}

int main (int argc, char **argv) {
    int token;

    if ( argc == 2 ) {
        yyin = fopen(argv[1], "r");
    }

    do {
        token = yylex();
        printf("token=%d, lexeme=\"%s\"\n",
                token, yytext);
    } while (token != T_eof);

    return 0;
}
