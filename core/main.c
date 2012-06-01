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
#include "quad.h"

extern int lineno;
extern AST_program ast;
extern FILE *yyin;
extern Quad_list q;
extern char *filename;

int ERROR(const char msg []) {
    fprintf(stderr, "ERROR, line %d: %s\n", lineno, msg);
    exit(1);
}

void yyerror(const char * msg)
{
  fprintf(stderr, "ERROR, line: %d: %s\n", lineno, msg);
  exit(1);
}

int main(int argc, char **argv) {
    if ( argc == 2 ) {
        yyin = fopen(argv[1], "r");

        if (yyin == NULL) {
            fprintf(stderr, "ERROR, could not open \"%s\": %s\n", argv[1], strerror(errno));
            exit(1);
        }

        filename = argv[1];
        /* filename = new(sizeof(char) * strlen(argv[1])); */
        /* strcpy(filename, argv[1]); */
    }
    else {
        yyin = stdin;

        filename = "stdin";
        /* filename = new(sizeof(char) * strlen("stdin")); */
        /* strcpy(filename, "stdin"); */
    }

    /* lexical and syntax analysis */
    yyparse();
    /* semantic analysis */
    AST_program_traverse(ast);
    /* quad generation */
    AST_program_quad_generate(ast);
    quad_list_print(stdout, q);

    return 0;
}
