#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "llama.h"
#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "pretty.h"
#include "semantic.h"

extern int lineno;
extern AST_program ast;
extern FILE *yyin;

int ERROR (const char msg []) {
    fprintf(stderr, "ERROR, line %d: %s\n", lineno, msg);
    exit(1);
}

void yyerror (const char * msg)
{
  fprintf(stderr, "ERROR, line: %d: %s\n", lineno, msg);
  exit(1);
}

int main (int argc, char **argv) {
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

    yyparse();
    /* AST_program_print(stdout, 0, ast); */
    AST_program_traverse(ast);

    /* int token; */
    /* do { */
    /*     token = yylex(); */
    /*     printf("token=%d, lexeme=\"%s\"\n", token, yytext); */
    /* } while (token != T_eof); */

    return 0;
}
