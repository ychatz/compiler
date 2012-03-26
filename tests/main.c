#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../tokens.h"
#define MAGIC_NUMBER 8 /* no idea how to calculate. hard-code for now */
#define LEX_ERROR -1
extern int lineno;

int ERROR(const char msg[]) {
    return LEX_ERROR;
}

void test_lexer_should_understand_integers(void) {
    yy_scan_string("31341234567890987654321\0"); 
    assert(yylex() == T_iconst);
    assert(yylex() == T_eof);
}

void test_lexer_should_understand_floats(void) {
    yy_scan_string("1833.1459\0"); 
    assert(yylex() == T_fconst);
    assert(yylex() == T_eof);
}

void test_lexer_should_understand_separated_keywords(void) {
    yy_scan_string("match int\nin     array\0"); 
    assert(yylex() == T_match);
    assert(yylex() == T_int);
    assert(yylex() == T_in);
    assert(yylex() == T_array);
    assert(yylex() == T_eof);
}

void test_lexer_should_not_care_about_inline_comments(void) {
    yy_scan_string("22 --this is a comment\n\0"); 
    assert(yylex() == T_iconst);
    assert(yylex() == T_eof);
}

void test_lexer_should_not_understand_strange_characters(void) {
    yy_scan_string("22`55\n\0"); 
    assert(yylex() == T_iconst);
    assert(yylex() == LEX_ERROR);
}

void test_lexer_should_count_lines(void) {
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
    int count, i;
    void (*lexer_tests[])(void) = {test_lexer_should_understand_integers,
                                   test_lexer_should_understand_floats,
                                   test_lexer_should_understand_separated_keywords,
                                   test_lexer_should_not_understand_strange_characters,
                                   test_lexer_should_not_care_about_inline_comments,
                                   test_lexer_should_count_lines};

    count = sizeof(lexer_tests)/MAGIC_NUMBER;

    printf("Testing lexical analyzer...    0%%");
    fflush(stdout);
    for (i=0; i<count; ++i) {
        lexer_tests[i]();
        system("sleep 0.2");
        printf("\b\b\b\b%3d%%", 100*(i+1)/count);
        fflush(stdout);
    }
    printf("\n");

    return 0;
}
