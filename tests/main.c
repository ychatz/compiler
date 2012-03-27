#include <stdio.h>
#include <stdlib.h>
#include "../tokens.h"
#define LEX_ERROR -1
#define assert(e) do { \
    if (!(e)) { \
    fprintf(stderr, "\n\n------------------------------\n\n"); \
    fprintf(stderr, "Test failed: %s\n" \
                    "  Condition: %s\n" \
                    "       File: %s\n" \
                    "       Line: %d\n", \
            __func__, #e,  __FILE__, __LINE__); \
    fprintf(stderr, "\n------------------------------\n\n"); \
    assert_failed(); \
    } \
} while(0);

extern int lineno;

int ERROR(const char msg[]) {
    return LEX_ERROR;
}

int assert_failed(void) {
    printf("* The tests failed! Aborting...\n");
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

void lexer_should_not_care_about_inline_comments(void) {
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

int main(void) {
    int size, i;
    void (*lexer_tests[])(void) = {lexer_should_understand_integers,
                                   lexer_should_understand_floats,
                                   lexer_should_understand_separated_keywords,
                                   lexer_should_not_understand_strange_characters,
                                   lexer_should_not_care_about_inline_comments,
                                   lexer_should_count_lines};

    size = sizeof(lexer_tests)/sizeof(lexer_tests[0]);

    printf("* Testing lexical analyzer...    0%%");
    fflush(stdout);
    for (i=0; i<size; ++i) {
        lexer_tests[i]();
        system("sleep 0.2");
        printf("\b\b\b\b%3d%%", 100*(i+1)/size);
        fflush(stdout);
    }
    printf("\n");

    printf("* The tests finished successfully!\n");

    return 0;
}
