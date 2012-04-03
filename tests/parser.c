#include "test.h"

int yyerror(const char msg[]) {
    return PARSE_ERROR;
}

void parser_should_accept_constant_definitions(void) {
    yy_scan_string("let x : in = 2");
    assert(yyparse()==1);
    yy_scan_string("let x : int = 2");
    assert(yyparse()==0);
    yy_scan_string("let x : in = 2");
    assert(yyparse()==1);
}

void parser_should_accept_variable_definitions(void) {
    yy_scan_string("let mutable x : int");
    assert(yyparse()==0);
}

void parser_run_tests(void) {
    int size, i;
    void (*parser_tests[])(void) = {
        parser_should_accept_constant_definitions,
        parser_should_accept_variable_definitions,
    };

    size = sizeof(parser_tests)/sizeof(parser_tests[0]);

    printf("* Testing syntax analyzer...     0%%");
    fflush(stdout);
    for (i=0; i<size; ++i) {
        parser_tests[i]();
        printf("\b\b\b\b%3d%%", 100*(i+1)/size);
        fflush(stdout);
    }
    printf("\n");

}
