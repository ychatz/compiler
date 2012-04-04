#include "test.h"

int yyerror(const char msg[]) {
    return PARSE_ERROR;
}

void parser_should_accept_constant_definitions(void) {
    yy_scan_string("let x : int = 2");
    assert(yyparse()==0);

    yy_scan_string("let xyz : float = 2.0");
    assert(yyparse()==0);
}

void parser_should_understand_expressions_with_vars(void) {
    yy_scan_string("let y = x mod 2");
    assert(yyparse()==0);

    yy_scan_string("let asd = !bcd * fgh");
    assert(yyparse()==0);
}

void parser_should_understand_expressions_with_paren(void) {
    yy_scan_string("let x = (5+1)/2");
    assert(yyparse()==0);
}

void parser_should_accept_variable_definitions(void) {
    yy_scan_string("let mutable x : int");
    assert(yyparse()==0);
}

void parser_should_accept_variable_array_definitions(void) {
    yy_scan_string("let mutable a [5] : int");
    assert(yyparse()==0);

    yy_scan_string("let mutable a [50, 3*3]");
    assert(yyparse()==0);

    yy_scan_string("let mutable a [9, 7/2, 4+1] : int");
    assert(yyparse()==0);
}

void parser_should_accept_function_definitions(void) {
    yy_scan_string("let inc x = x + 1");
    assert(yyparse()==0);

    yy_scan_string("let inc (x: int) : int = x + 1");
    assert(yyparse()==0);

    yy_scan_string("let odd (x: int) : bool = x mod 2 <> 0");
    assert(yyparse()==0);
}

void parser_run_tests(void) {
    int size, i;
    void (*parser_tests[])(void) = {
        parser_should_accept_constant_definitions,
        parser_should_understand_expressions_with_vars,
        parser_should_understand_expressions_with_paren,
        parser_should_accept_variable_definitions,
        parser_should_accept_variable_array_definitions,
        parser_should_accept_function_definitions,
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
