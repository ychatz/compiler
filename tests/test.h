#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../core/llama.h"
#include "../core/ast.h"
#include "../core/lexer.h"
#include "../core/parser.h"
#include "../core/semantic.h"

#include "helpers.h"

#define assert(e) do { \
    if (!(e)) { \
        assert_failed(__func__, #e, __FILE__, __LINE__); \
    } \
} while(0);

extern int numErrors;
extern int numWarnings;
extern int lineno;
extern AST_program ast;

void assert_failed(const char *, const char *, const char *, const int);

#define LEX_ERROR -1
int ERROR(const char *);
void lexer_run_tests();

#define PARSE_ERROR 1
int yyerror(char const *s);
void parser_run_tests();
