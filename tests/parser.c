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

void parser_should_recognize_the_semicolon_separator(void) {
    yy_scan_string("let a = x := !y; y := t");
    assert(yyparse()==0);

    yy_scan_string("let a = x := !y; y := t; q = 9");
    assert(yyparse()==0);
}

void parser_should_recognize_type_definitions(void) {
    yy_scan_string("type color = Red | Green | Blue");
    assert(yyparse()==0);

    yy_scan_string("type number = Integer of int | Real of float | Complex of float float");
    assert(yyparse()==0);

    yy_scan_string("type list = Nil | Cons of int list");
    assert(yyparse()==0);
}

void parser_should_recognize_simple_rec_definitions(void) {
    yy_scan_string("let rec fact n = if n = 0 then 1 else n * fact (n-1)");
    assert(yyparse()==0);
}

void parser_should_recognize_multiple_rec_definitions(void) {
    yy_scan_string("let rec even n = if n = 0 then true else odd (n-1)\n"
                   "and odd n = if n = 0 then false else even (n-1)\n"
                   "and triple = true");
    assert(yyparse()==0);
}

void parser_should_recognize_multiple_type_definitions(void) {
    yy_scan_string("type color = Red | Green | Blue\n"
                   "and medal = Gold | Silver | Bronze\n"
                   "and male = Alpha | Beta | Gamma | Delta");
    assert(yyparse()==0);
}

void parser_should_recognize_match_expressions(void) {
    yy_scan_string("let main = match num with\n"
                   "Integer i -> print_int i\n"
                   "| Real f -> print_float f\n"
                   "| Complex re 0.0 -> print_float re\n"
                   "| Complex 0.0 im -> print_string \"j\"; print_float im\n"
                   "| Complex re im -> print_float re;\n"
                   "print_string (if im > 0.0 then \"+j\" else \"-j\");\n"
                   "print_float (abs_float im)\n"
                   "end\n");
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
        parser_should_recognize_the_semicolon_separator,
        parser_should_recognize_type_definitions,
        parser_should_recognize_simple_rec_definitions,
        parser_should_recognize_multiple_rec_definitions,
        parser_should_recognize_multiple_type_definitions,
        parser_should_recognize_match_expressions,
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
