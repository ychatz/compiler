#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"

extern char *yytext;
extern int lineno;

int ERROR (const char msg []) {
    fprintf(stderr, "ERROR, line %d: %s\n",
    lineno, msg);
    exit(1);
}

int main () {
    int token;

    do {
        token = yylex();
        printf("token=%d, lexeme=\"%s\"\n",
                token, yytext);
    } while (token != T_eof);

    return 0;
}
