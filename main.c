#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"

extern char *yytext;

int main ()
{
  int token;

  do {
    token = yylex();
    printf("token=%d, lexeme=\"%s\"\n",
           token, yytext);
  } while (token != T_eof);
  return 0;
}
