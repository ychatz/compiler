#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../tokens.h"
#include "helpers.h"
#define LEX_ERROR -1
#define assert(e) do { \
    if (!(e)) { \
        assert_failed(__func__, #e, __FILE__, __LINE__); \
    } \
} while(0);

extern int lineno;

int ERROR(const char msg[]) {
    return LEX_ERROR;
}

void assert_failed(const char *func, const char *cond, const char *file,
                   const int line ) {
    char *test_name;
    size_t test_name_size = strlen(func) + 1;

    test_name = (char *)(malloc(sizeof(char) * test_name_size));
    memcpy(test_name, func, test_name_size * sizeof(char));
    strreplace(test_name, '_', ' ');

    fprintf(stderr, "\n\n------------------------------\n\n");
    fprintf(stderr, "Test failed: %s\n"
                    "  Condition: %s\n"
                    "       File: %s\n"
                    "       Line: %d\n",
                    test_name, cond, file, line);
    fprintf(stderr, "\n------------------------------\n\n");
    printf("* The tests failed! Aborting...\n");

    free(test_name);
    abort();
}

void lexer_should_understand_integers(void) {
    yy_scan_string("31341234567890987654321\0"); 
    assert(yylex() == T_iconst);
    assert(yylex() == T_eof);
}

void lexer_should_understand_floats(void) {
    yy_scan_string("1833.1459 1.203e-7 123.0e22 918.129e+33\0"); 
    assert(yylex() == T_fconst);
    assert(yylex() == T_fconst);
    assert(yylex() == T_fconst);
    assert(yylex() == T_fconst);
    assert(yylex() == T_eof);
}

void lexer_should_understand_separated_keywords(void) {
    yy_scan_string("match int\nin     array\0"); 
    assert(yylex() == T_match);
    assert(yylex() == T_int);
    assert(yylex() == T_in);
    assert(yylex() == T_array);
    assert(yylex() == T_eof);
}

void lexer_should_support_inline_comments(void) {
    yy_scan_string("22 --this is a comment\n\0"); 
    assert(yylex() == T_iconst);
    assert(yylex() == T_eof);
}

void lexer_should_not_understand_strange_characters(void) {
    yy_scan_string("22`55\n\0"); 
    assert(yylex() == T_iconst);
    assert(yylex() == LEX_ERROR);
}

void lexer_should_count_lines(void) {
    lineno = 1;
    yy_scan_string("let\n"
                   "source target\n"
                   "\n"
                   "(* this is a\n"
                   " multiline \n"
                   "comment *)  \n"
                   "p$rint_string source;\n"
                   "print_string target;\n\0"); 
    assert(yylex() == T_let);
    assert(yylex() == T_id);
    assert(yylex() == T_id);
    assert(yylex() == T_id);
    assert(yylex() == LEX_ERROR);
    assert(lineno == 7);
}

void lexer_should_support_nested_comments(void) {
    yy_scan_string("(* int\n"
                   "(* char\n"
                   "array *)\n"
                   "mutable *)\0");
    assert(yylex() == T_eof);

    yy_scan_string("(* (* *) (* *) (* (* char *) *) *)\0");
    assert(yylex() == T_eof);
}

void lexer_should_support_all_chars_inside_comments(void) {
    yy_scan_string("(* * ( ) ** asd  *hello* *) int"
                   "(** multi--line\n"
                   " ** multi--asterisk\n"
                   " **) int\0");
    assert(yylex() == T_int);
    assert(yylex() == T_int);
    assert(yylex() == T_eof);

    yy_scan_string("--- three dashes int\n"
                   "-- dash dash -- int\n\0");
    assert(yylex() == T_eof);
}

void lexer_should_report_unterminated_comments(void) {
    yy_scan_string("(* (* a test *) int\0");
    assert(yylex() == LEX_ERROR);
}

void lexer_should_report_suffix_on_integer_constants(void) {
    yy_scan_string("212sda\0");
    assert(yylex() == LEX_ERROR);
}

void lexer_should_report_suffix_on_float_constants(void) {
    yy_scan_string("212.01e-1sda\0");
    assert(yylex() == LEX_ERROR);
}

void lexer_should_accept_strings(void) {
    yy_scan_string("\" this is a string that contains \\x012 \\nescape codes\\t \\r\\0\"\0");
    assert(yylex() == T_sconst);
}

void lexer_should_report_unterminated_strings(void) {
    yy_scan_string("\" this is \0");
    assert(yylex() == LEX_ERROR);
}

int main(void) {
    int size, i;
    void (*lexer_tests[])(void) = {
        lexer_should_understand_integers,
        lexer_should_understand_floats,
        lexer_should_understand_separated_keywords,
        lexer_should_not_understand_strange_characters,
        lexer_should_count_lines,
        lexer_should_support_inline_comments,
        lexer_should_support_nested_comments,
        lexer_should_support_all_chars_inside_comments,
        lexer_should_report_unterminated_comments,
        lexer_should_report_suffix_on_integer_constants,
        lexer_should_report_suffix_on_float_constants,
        lexer_should_accept_strings,
        lexer_should_report_unterminated_strings
    };

    size = sizeof(lexer_tests)/sizeof(lexer_tests[0]);

    printf("* Testing lexical analyzer...    0%%");
    fflush(stdout);
    for (i=0; i<size; ++i) {
        lexer_tests[i]();
        yylex_destroy(); /* avoid memory leaks */
        system("sleep 0.05");
        printf("\b\b\b\b%3d%%", 100*(i+1)/size);
        fflush(stdout);
    }
    printf("\n");

    printf("* The tests finished successfully!\n");

    return 0;
}
