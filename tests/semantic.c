#include "test.h"

void semantic_should_accept_constant_definitions(void) {
    yy_scan_string("let main = match num with\n"
                   "Integer i -> print_int i\n"
                   "| Real f -> print_float f\n"
                   "| Complex re 0.0 -> print_float re\n"
                   "| Complex 0.0 im -> print_string \"j\"; print_float im\n"
                   "| Complex re im -> print_float re;\n"
                   "print_string (if im > 0.0 then \"+j\" else \"-j\");\n"
                   "print_float (abs_float im)\n"
                   "en\n");
    assert(yyparse()==0);
}

void semantic_run_tests(void) {
    int size, i;
    void (*parser_tests[])(void) = {
        semantic_should_accept_constant_definitions,
    };

    size = sizeof(parser_tests)/sizeof(parser_tests[0]);

    printf("* Testing semantic analyzer...   0%%");
    fflush(stdout);
    for (i=0; i<size; ++i) {
        parser_tests[i]();
        printf("\b\b\b\b%3d%%", 100*(i+1)/size);
        fflush(stdout);
    }
    printf("\n");

}
