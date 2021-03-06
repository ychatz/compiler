%{
#include <string.h>
#include "ast.h"
#include "parser.h"
#include "llama.h"
#include "general.h"
int lineno=1;
int comment_level=0;
int ERROR (const char msg []);
%}

/* decimal digits */
D [0-9]
/* hexedecimal digits */
H [0-9a-fA-F]
/* escape sequences */
ES  \\([ntr0\\\'\"]|x{H}{H})
/* printable characters */
PR [ !#-&\(-\[\]-~]
/* letters */
L [a-zA-Z]

%option nounput

%x COMMENT
%%
"eof"                               { return T_eof; }
"and"                               { return T_and; }
"array"                             { return T_array; }
"begin"                             { return T_begin; }
"bool"                              { return T_bool; }
"char"                              { return T_char; }
"delete"                            { return T_delete; }
"dim"                               { return T_dim; }
"do"                                { return T_do; }
"done"                              { return T_done; }
"downto"                            { return T_downto; }
"else"                              { return T_else; }
"end"                               { return T_end; }
"false"                             { return T_false; }
"float"                             { return T_float; }
"for"                               { return T_for; }
"if"                                { return T_if; }
"in"                                { return T_in; }
"int"                               { return T_int; }
"let"                               { return T_let; }
"match"                             { return T_match; }
"mod"                               { return T_mod; }
"mutable"                           { return T_mutable; }
"new"                               { return T_new; }
"not"                               { return T_not; }
"of"                                { return T_of; }
"rec"                               { return T_rec; }
"ref"                               { return T_ref; }
"then"                              { return T_then; }
"to"                                { return T_to; }
"true"                              { return T_true; }
"type"                              { return T_type; }
"unit"                              { return T_unit; }
"while"                             { return T_while; }
"with"                              { return T_with; }

{D}+{L}+                            { return ERROR("Invalid suffix on integer constant\n"); }
{D}+                                { yylval.intval = atoi(yytext); return T_iconst; }
{D}+\.{D}+([eE][\+\-]?{D}+)?{L}+    { return ERROR("Invalid suffix on float constant\n");  }
{D}+\.{D}+([eE][\+\-]?{D}+)?        { yylval.floatval = atof(yytext); return T_fconst; }
\'({PR}|{ES})\'                     { yylval.charval = strdup(yytext); return T_cconst; }
\"({PR}|{ES})*\n                    { return ERROR("Missing terminating \" character\n"); }
\"({PR}|{ES})*\"                    { yylval.stringval = strdup(yytext); return T_sconst; }

[a-z][A-Za-z0-9_]*                  { yylval.identifier = id_make(yytext); return T_id; }
[A-Z][A-Za-z0-9_]*                  { yylval.identifier = id_make(yytext); return T_constructor; }

"->"                                { return T_op_arrow; }
"+."                                { return T_op_fadd; }
"-."                                { return T_op_fsub; }
"*."                                { return T_op_fmul; }
"/."                                { return T_op_fdiv; }
"**"                                { return T_op_fexp; }
"&&"                                { return T_op_and; }
"||"                                { return T_op_or; }
"<>"                                { return T_op_struct_neq; }
"<="                                { return T_op_leq; }
">="                                { return T_op_geq; }
"=="                                { return T_op_eq; }
"!="                                { return T_op_phys_neq; }
":="                                { return T_op_assign; }
[=\|\+\-\*\/\!\;\<\>\(\)\[\]\,\:]   { return yytext[0]; }

"--".+\n                            { lineno++; }
"(*"                                { BEGIN(COMMENT); comment_level++; }
<COMMENT>"(*"                       { comment_level++; }
<COMMENT>"*)"                       { if ( !--comment_level ) BEGIN(INITIAL); }
<COMMENT>\n                         { lineno++; }
<COMMENT>"("
<COMMENT>"*"
<COMMENT>[^(*\n]+
<COMMENT><<EOF>>                    { return ERROR("Unterminated comment\n"); }

\n                                  { lineno++; }
[ \t\r]
<<EOF>>                             { return T_eof; }
.                                   { return ERROR("Lexical error\n"); }
%%
