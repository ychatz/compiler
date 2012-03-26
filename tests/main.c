#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../tokens.h"
#define MAGIC_NUMBER 8 /* no idea how to calculate. hard-code for now */

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

int main(void) {
    int count, i;
    void (*lexer_tests[])(void) = {test_lexer_should_understand_integers,
                                   test_lexer_should_understand_floats,
                                   test_lexer_should_understand_separated_keywords};

    count = sizeof(lexer_tests)/MAGIC_NUMBER;

    printf("Testing lexical analyzer...    0%%");
    fflush(stdout);
    for (i=0; i<count; ++i) {
        lexer_tests[i]();
        system("sleep 0.5");
        printf("\b\b\b\b%3d%%", 100*(i+1)/count);
        fflush(stdout);
    }
    printf("\n");

    return 0;
}
