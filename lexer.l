%{
#include <stdio.h>
#include <stdlib.h>

#define T_eof           0
#define T_and           1000
#define T_array         1001
#define T_begin         1002
#define T_bool          1003
#define T_char          1004
#define T_delete        1005
#define T_dim           1006
#define T_do            1007
#define T_done          1008
#define T_downto        1009
#define T_else          1010
#define T_end           1011
#define T_false         1012
#define T_float         1013
#define T_for           1014
#define T_if            1015
#define T_in            1016
#define T_int           1017
#define T_let           1018
#define T_match         1019
#define T_mod           1020
#define T_mutable       1021
#define T_new           1022
#define T_not           1023
#define T_of            1024
#define T_rec           1025
#define T_ref           1026
#define T_then          1027
#define T_to            1028
#define T_true          1029
#define T_type          1030
#define T_unit          1031
#define T_while         1032
#define T_with          1033
#define T_iconst        1034
#define T_fconst        1035
#define T_cconst        1036
#define T_sconst        1037

#define T_op_arrow      2000
#define T_op_fadd       2001
#define T_op_fsub       2002
#define T_op_fmul       2003
#define T_op_fdiv       2004
#define T_op_fexp       2005
#define T_op_and        2006
#define T_op_or         2007
#define T_op_struct_neq 2008
#define T_op_leq        2009
#define T_op_geq        2010
#define T_op_eq         2011
#define T_op_phys_neq   2012
#define T_op_assign     2013

#define T_id            3000
#define T_constructor   3001
%}

D [0-9]
H [0-9a-fA-F]
ES  \\([ntr0\\\'\"]|x{H}{H})
PR [ !#-&\(-\[\]-~]

%x COMMENT
%%
"eof"                        { return T_eof;     }
"and"                        { return T_and;     }
"array"                      { return T_array;   }
"begin"                      { return T_begin;   }
"bool"                       { return T_bool;    }
"char"                       { return T_char;    }
"delete"                     { return T_delete;  }
"dim"                        { return T_dim;     }
"do"                         { return T_do;      }
"done"                       { return T_done;    }
"downto"                     { return T_downto;  }
"else"                       { return T_else;    }
"end"                        { return T_end;     }
"false"                      { return T_false;   }
"float"                      { return T_float;   }
"for"                        { return T_for;     }
"if"                         { return T_if;      }
"in"                         { return T_in;      }
"int"                        { return T_int;     }
"let"                        { return T_let;     }
"match"                      { return T_match;   }
"mod"                        { return T_mod;     }
"mutable"                    { return T_mutable; }
"new"                        { return T_new;     }
"not"                        { return T_not;     }
"of"                         { return T_of;      }
"rec"                        { return T_rec;     }
"ref"                        { return T_ref;     }
"then"                       { return T_then;    }
"to"                         { return T_to;      }
"true"                       { return T_true;    }
"type"                       { return T_type;    }
"unit"                       { return T_unit;    }
"while"                      { return T_while;   }
"with"                       { return T_with;    }

{D}+                         { return T_iconst;  }
{D}+\.{D}+([eE][\+\-]?{D}+)? { return T_fconst;  }
\'({PR}|{ES})\'              { return T_cconst;  }
\"({PR}|{ES})*\"             { return T_sconst;  }

[a-z][A-Za-z0-9_]*           { return T_id; }
[A-Z][A-Za-z0-9_]*           { return T_constructor; }

"->"                         { return T_op_arrow; }
"+."                         { return T_op_fadd; }
"-."                         { return T_op_fsub; }
"*."                         { return T_op_fmul; }
"/."                         { return T_op_fdiv; }
"**"                         { return T_op_fexp; }
"&&"                         { return T_op_and; }
"||"                         { return T_op_or; }
"<>"                         { return T_op_struct_neq; }
"<="                         { return T_op_leq; }
">="                         { return T_op_geq; }
"=="                         { return T_op_eq; }
"!="                         { return T_op_phys_neq; }
":="                         { return T_op_assign; }
[=\|\+\-\*\/\!\;\<\>\(\)\[\]\,\:] { return yytext[0]; }

"--".+\n
"(*" { BEGIN(COMMENT); }
<COMMENT>"*)" { BEGIN(INITIAL); }
<COMMENT>\n { /*lineno++;*/ }
<COMMENT>"*" { /* nothing */ }
<COMMENT>[^*\n]+ { /* nothing */ }

[ \n\t\r]
<<EOF>>                      { return T_eof; }
.                            { return 1; }
%%

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
