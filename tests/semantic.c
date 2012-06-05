#include "test.h"

int expectError;
int numErrors;
int numWarnings;
char __func_name[100], __file_name[100];
int __line_number;

int fatal(const char *fmt, ...) {
    char s[100];
    va_list ap;
    numErrors++;
    if ( !expectError ) {
        va_start(ap, fmt);
        vsprintf(s, fmt, ap);
        va_end(ap);
        assert_failed(__func_name, s, __file_name, __line_number);
        abort();
    }
}
int error(const char *fmt, ...) {
    char s[100];
    va_list ap;
    numErrors++;
    if ( !expectError ) {
        va_start(ap, fmt);
        vsprintf(s, fmt, ap);
        va_end(ap);
        assert_failed(__func_name, s, __file_name, __line_number);
        abort();
    }
}

int internal_raw(const char *fmt, ...) {
    char s[100];
    va_list ap;
    numErrors++;
    if ( !expectError ) {
        va_start(ap, fmt);
        vsprintf(s, fmt, ap);
        va_end(ap);
        assert_failed(__func_name, s, __file_name, __line_number);
        abort();
    }
}

int message(const char *fmt, ...) {
}

#define static_analysis(errorsExpected) do { \
    assert(yyparse()==0); \
    numErrors = 0; \
    expectError = errorsExpected; \
    strcpy(__func_name, __func__); \
    strcpy(__file_name, __FILE__); \
    __line_number = __LINE__; \
    AST_program_traverse(ast); \
    assert(numErrors == expectError); \
} while(0);

void semantic_should_check_for_type_mismatch_in_function_result_values(void) {
    yy_scan_string("let x : int = '2'\n");
    static_analysis(1);

    yy_scan_string("let x : char = 's'; 3\n");
    static_analysis(1);

    yy_scan_string("let x : int = 's'; 3\n");
    static_analysis(0);
}

void semantic_should_check_for_type_mismatch_in_function_parameters(void) {
    yy_scan_string("let x (s: int) : int = s\n");
    static_analysis(0);

    yy_scan_string("let x (s: int) : char = s\n");
    static_analysis(1);
}

void semantic_should_detect_assign_of_non_ref_left_operand(void) {
    yy_scan_string("let x : char = 'c'\n"
                   "let fun (r : char) = x := r");
    static_analysis(1);
}

void semantic_should_allow_assign_of_ref_left_operand(void) {
    yy_scan_string("let mutable x : char\n"
                   "let fun (r : char) = x := r");
    static_analysis(0);
}

/* void semantic_should_apply_type_inference(void) { */
/*     yy_scan_string("let fun x = x := 2; x := 'c'\n"); */
/*     static_analysis(1); */
/* } */

void semantic_run_tests(void) {
    int size, i;
    void (*parser_tests[])(void) = {
        semantic_should_check_for_type_mismatch_in_function_result_values,
        semantic_should_check_for_type_mismatch_in_function_parameters,
        semantic_should_detect_assign_of_non_ref_left_operand,
        semantic_should_allow_assign_of_ref_left_operand,
        /* semantic_should_apply_type_inference */
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
