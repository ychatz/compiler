%{
#include "lexer.h"
#include "llama.h"
#include "ast.h"

void yyerror (const char * msg);
extern int lineno;
AST_program ast;
%}

%start program
%expect 23

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

%nonassoc "let" "in"
%left ';'
%nonassoc "if" "then"
%nonassoc ":="
%left "||"
%left "&&"
%nonassoc '=' "<>" '>' '<' "<=" ">=" "==" "!="
%left '+' '-' "+." "-."
%left '*' '/' "*." "/." "mod"
%right "**"
%nonassoc "not" "delete" INT_POS_SIGN INT_NEG_SIGN FLOAT_POS_SIGN FLOAT_NEG_SIGN
%nonassoc '!'
%nonassoc "new"

/* type definition precedence */
/* set according to http://moodle.softlab.ntua.gr/mod/forum/discuss.php?d=317 */
%right "->"
%nonassoc "of"
%nonassoc "ref"
%%

program:
      definition_list { ast = ast_program($1);  }
;

definition_list:
	/* nothing */ { $$ = NULL;  }
       | let_definition definition_list { $$ = ast_ltdef_list_let($1, $2); }
       | type_definition definition_list { $$ = ast_ltdef_list_type($1, $2); } 
;



let_definition: "let" many_definitions { $$ = ast_letdef(false, $2);}
              | "let" "rec" many_definitions { $$ = ast_letdef(true, $3);}
;

many_definitions: definition { $$ = ast_def_list ($1, NULL); } 
                | definition "and" many_definitions { $$ = ast_def_list($1, $3);}
;

definition: "id" parameter_list '=' expr { $$ = ast_def_normal($1, $2, NULL, $4); }
          | "id" parameter_list ':' type '=' expr { $$ = ast_def_normal($1, $2, $3, $6); }
          | "mutable" "id" { $$ = ast_def_mutable($2, NULL, NULL); }
          | "mutable" "id" ':' type { $$ = ast_def_mutable($2, NULL, $3); }
          | "mutable" "id" array_size_def { $$ = ast_def_mutable($2, $3, NULL); }
          | "mutable" "id" array_size_def ':' type { $$ = ast_def_mutable($2, $3, $5); }
;

parameter_list: { $$ = NULL; }
              | parameter parameter_list
;

parameter: "id"
         | '(' "id" ':' type ')'
;

array_size_def: '[' multi_expr ']' { $$ = $2; }
;


multi_expr: expr
          | expr ',' multi_expr
;

type_definition: "type" many_type_definitions
;

many_type_definitions: "id" '=' constr_list
                     | "id" '=' constr_list "and" many_type_definitions
;

constr_list: constructor
           | constructor '|' constr_list
;

constructor: "constructor"
           | "constructor" "of" many_types
;

many_types: type
          | type many_types
;

type: "unit"
    | "int"
    | "char"
    | "bool"
    | "float"
    | '(' type ')'
    | type "ref"
    | type "->" type
    | "array" '[' multi_asterisks ']' "of" type
    | "array" "of" type
    | "id"
;

multi_asterisks: '*'
               | '*' ',' multi_asterisks
;

clause_list: pattern "->" expr
           | pattern "->" expr '|' clause_list
;

pattern_high: '+' "int_const" %prec INT_POS_SIGN
            | '-' "int_const" %prec INT_NEG_SIGN
            | "int_const"
            | "+." "float_const" %prec FLOAT_POS_SIGN
            | "-." "float_const" %prec FLOAT_NEG_SIGN
            | "float_const"
            | "char_const"
            | "true"
            | "false"
            | "id"
            | '(' pattern ')'
;

pattern: pattern_high
       | "constructor" many_patterns_high
;

many_patterns_high:
                  | pattern_high many_patterns_high
;

/* http://moodle.softlab.ntua.gr/mod/forum/discuss.php?d=320 */
many_expr_high: expr_high
              | expr_high many_expr_high
;

expr_high: '!' expr_high
         | '(' expr ')'
         | '(' ')'
         | "int_const" { $$ = ast_expr_iconst($1); }
         | "float_const"
         | "char_const"
         | "string_const"
         | "true"
         | "false"
         | "id"
         | "id" array_size_def
         | "constructor"
;

expr: "not" expr
    | '+' expr %prec INT_POS_SIGN
    | '-' expr %prec INT_NEG_SIGN
    | "+." expr %prec FLOAT_POS_SIGN
    | "-." expr %prec FLOAT_NEG_SIGN
    | expr '+' expr { $$ = ast_expr_binop($1, ast_binop_plus, $3); }
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | expr "mod" expr
    | expr "+." expr
    | expr "-." expr
    | expr "*." expr
    | expr "/." expr
    | expr "**" expr
    | expr '=' expr
    | expr "<>" expr
    | expr '<' expr
    | expr '>' expr
    | expr "<=" expr
    | expr ">=" expr
    | expr "==" expr
    | expr "!=" expr
    | expr "&&" expr
    | expr "||" expr
    | expr ":=" expr
    | expr ';' expr
    | let_definition "in" expr
    | "while" expr "do" expr "done"
    | "for" "id" '=' expr "to" expr "do" expr "done"
    | "for" "id" '=' expr "downto" expr "do" expr "done"
    | "dim" "id"
    | "dim" "int_const" "id"
    | "new" type
    | "delete" type
    | "id" many_expr_high
    | "constructor" many_expr_high
    | "if" expr "then" expr
    | "if" expr "then" expr "else" expr
    | "begin" expr "end"
    | "match" expr "with" clause_list "end"
    | expr_high
;

%%
