%{
#include "lexer.h"
#include "llama.h"

void yyerror (const char * msg);
%}

%start program

%token T_and             "and"
%token T_array           "array"
%token T_begin           "begin"
%token T_bool            "bool"
%token T_char            "char"
%token T_delete          "delete"
%token T_dim             "dim"
%token T_do              "do"
%token T_done            "done"
%token T_downto          "downto"
%token T_else            "else"
%token T_end             "end"
%token T_false           "false"
%token T_float           "float"
%token T_for             "for"
%token T_if              "if"
%token T_in              "in"
%token T_int             "int"
%token T_let             "let"
%token T_match           "match"
%token T_mod             "mod"
%token T_mutable         "mutable"
%token T_new             "new"
%token T_not             "not"
%token T_of              "of"
%token T_rec             "rec"
%token T_ref             "ref"
%token T_then            "then"
%token T_to              "to"
%token T_true            "true"
%token T_type            "type"
%token T_unit            "unit"
%token T_while           "while"
%token T_with            "with"
%token T_iconst          "int_const"
%token T_fconst          "float_const"
%token T_cconst          "char_const"
%token T_sconst          "string_const"

%token T_op_arrow        "->"
%token T_op_fadd         "+."
%token T_op_fsub         "-."
%token T_op_fmul         "*."
%token T_op_fdiv         "/."
%token T_op_fexp         "**"
%token T_op_and          "&&"
%token T_op_or           "||"
%token T_op_struct_neq   "<>"
%token T_op_leq          "<="
%token T_op_geq          ">="
%token T_op_eq           "=="
%token T_op_phys_neq     "!="
%token T_op_assign       ":="

%token T_id              "id"
%token T_constructor     "constructor"

%left '+' '-' "+." "-."
%left '*' '/' "*." "/." "mod"
%right "**"
%%

program:
       | let_definition program
;

let_definition: "let" "id" '=' expr
              | "let" "id" ':' type '=' expr
;

type: "unit"
    | "int"
    | "char"
    | "bool"
    | "float"
;

expr: "int_const"
    | "float_const"
    | expr '+' expr
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | expr "mod" expr
    | expr "+." expr
    | expr "-." expr
    | expr "*." expr
    | expr "/." expr
    | expr "**" expr
;

%%
