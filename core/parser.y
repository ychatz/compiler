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
%token <RepInt> T_iconst          "int_const"
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



%union {
   /* ... */

   AST_letdef        letdef;
   AST_typedef       typedef;
   AST_def           def;
   AST_tdef          tdef;
   AST_constr        constr;
   AST_par           par;
   AST_expr          expr;
   AST_clause        clause;
   AST_pattern       pattern;

   AST_ltdef_list    ltdef_list;
   AST_def_list      def_list;
   AST_tdef_list     tdef_list;
   AST_constr_list   constr_list;
   AST_par_list      par_list;
   AST_expr_list     expr_list;
   AST_clause_list   clause_list;
   AST_pattern_list  pattern_list;

   AST_expr_high     expr_high;
   AST_pattern_high  pattern_high;

   

   /* Type_list */
   /* ... */
}

%type<program> program
%type<letdef> let_definition
%type<typedef> type_definition
%type<def> definition
%type<tdef> t_definition
%type<constr> constructor
%type<par> parameter
%type<expr> expr
%type<clause> clause
%type<pattern> pattern

%type<ltdef_list> definition_list
%type<def_list> many_definitions
%type<tdef_list> many_type_definitions
%type<constr_list> constr_list
%type<par_list> parameter_list
%type<expr_list> many_expr_high
%type<clause_list> clause_list
%type<pattern_list> many_patterns_high



%%

program:
      definition_list 				{ ast = ast_program($1); }
;

definition_list:
	/* nothing */ 				{ $$ = NULL;  }
       | let_definition definition_list 	{ $$ = ast_ltdef_list_let($1, $2); }
       | type_definition definition_list 	{ $$ = ast_ltdef_list_type($1, $2); } 
;

let_definition: "let" many_definitions 		{ $$ = ast_letdef(false, $2);}
              | "let" "rec" many_definitions 	{ $$ = ast_letdef(true, $3);}
;

many_definitions: definition 				{ $$ = ast_def_list ($1, NULL); } 
                | definition "and" many_definitions 	{ $$ = ast_def_list($1, $3);}
;e 

definition: "id" parameter_list '=' expr 		{ $$ = ast_def_normal($1, $2, NULL, $4); }
          | "id" parameter_list ':' type '=' expr 	{ $$ = ast_def_normal($1, $2, $4, $6); }
          | "mutable" "id" 				{ $$ = ast_def_mutable($2, NULL, NULL); }
          | "mutable" "id" ':' type 			{ $$ = ast_def_mutable($2, NULL, $4); }
          | "mutable" "id" "[" multi_expr "]" 		{ $$ = ast_def_mutable($2, $4, NULL); }
          | "mutable" "id" "[" multi_expr "]" ':' type 	{ $$ = ast_def_mutable($2, $4, $5); }
;

parameter_list: 
		/* nothing */			{ $$ = NULL; }
              | parameter parameter_list 	{ $$ = ast_par_list($1, $2); }
;

parameter: "id" 			{ $$ = ast_par($1, NULL); }
         | '(' "id" ':' type ')' 	{ $$ = ast_par($2, $4); }
;

multi_expr: expr 			{ $$ = $1; }
          | expr ',' multi_expr  	{ $$ = ast_expr_list ($1, $3); }  
;

type_definition: "type" many_type_definitions 		{ $$ = ast_typedef($2); }
;

many_type_definitions: t_definition 					{ $$ = $1; }		
                     | t_definition "and" many_type_definitions 	{ $$ = ast_tdef_list ($1, $3); } 
;

t_definition: "id" '=' constr_list 			{ $$ = ast_tdef($1, $3); }
;

constr_list: constructor 				{ $$ = $1; }
           | constructor '|' constr_list 		{ $$ = ast_constr($1, $3); }
;

constructor: "constructor"  				{ $$ = ast_constr ($1, NULL); }
           | "constructor" "of" many_types    		{ $$ = ast_constr ($1, $3); }
;

many_types: type			{ $$ = $1; }	
          | type many_types		{ $$ = type_list ($1, $2); }	
;

type: "unit" 						{ $$ = type_unit(); }
    | "int"						{ $$ = type_int(); }
    | "char"						{ $$ = type_char(); }
    | "bool"						{ $$ = type_bool(); }
    | "float"						{ $$ = type_float(); }
    | '(' type ')'					{ $$ = $2 }
    | type "ref"					{ $$ = type_ref($1); }
    | type "->" type					{ $$ = type_func($1, $3); }
    | "array" '[' multi_asterisks ']' "of" type		{ $$ = type_array(); } 
    | "array" "of" type					{ $$ = type_array(0, $3); }
    | "id"						{ $$ = type_id($1); }
;

multi_asterisks: '*'
               | '*' ',' multi_asterisks
;

clause_list: clause  				{ $$ = ast_clause_list ($1, NULL); } 
           | clause '|' clause_list 		{ $$ = ast_clause_list ($1, $3); }
;

clause: pattern "->" expr 			{ $$ = ast_clause ($1, $3); }
;

pattern_high: '+' "int_const" %prec INT_POS_SIGN	 	{ $$ = ast_pattern_iconst($2); }
            | '-' "int_const" %prec INT_NEG_SIGN 		{ $$ = ast_pattern_iconst($2); }
            | "int_const" 					{ $$ = ast_pattern_iconst($1); }
            | "+." "float_const" %prec FLOAT_POS_SIGN 		{ $$ = ast_pattern_fconst($2); }
            | "-." "float_const" %prec FLOAT_NEG_SIGN 		{ $$ = ast_pattern_fconst($2); }
            | "float_const" 					{ $$ = ast_pattern_fconst($1); }
            | "char_const" 					{ $$ = ast_pattern_fconst($1); }
            | "true" 						{ $$ = ast_pattern_true(); }
            | "false" 						{ $$ = ast_pattern_false(); }
            | "id" 						{ $$ = ast_pattern_id($1); } 
            | '(' pattern ')' 					{ $$ = $2; } 
;

pattern: pattern_high 				{ $$ = $1; } 
       | "constructor" many_patterns_high 	{ $$ = ast_pattern_Id($1, $2); }
;

many_patterns_high:
		  /* nothing */				{ $$ = NULL; } 
                  | pattern_high many_patterns_high 	{ $$ = ast_pattern_list($1, $2); }
;

/* http://moodle.softlab.ntua.gr/mod/forum/discuss.php?d=320 */
many_expr_high: expr_high  			{ $$ = ast_expr_list($1, NULL); } 
              | expr_high many_expr_high 	{ $$ = ast_expr_list($1, $2); } 
;

expr_high: '!' expr_high		{ $$ = ast_expr_unop (ast_unop_exclam, $2); } 
         | '(' expr ')'			{ $$ = $2; }
         | '(' ')' 			{ $$ = ast_expr_unit (); }
         | "int_const" 			{ $$ = ast_expr_iconst($1); }
         | "float_const" 		{ $$ = ast_expr_fconst($1); }
         | "char_const" 		{ $$ = ast_expr_cconst($1); }
         | "string_const" 		{ $$ = ast_expr_strlit($1); }
         | "true" 			{ $$ = ast_expr_true(); }
         | "false" 			{ $$ = ast_expr_false(); }
         | "id" 			{ $$ = ast_expr_id($1); }
         | "id" "[" multi_expr "]" 	/* ??? ast_expr_arrel (Identifier id, AST_expr_list l) ??? */
         | "constructor" 		{ $$ = ast_expr_Id($1); }
;

expr: "not" expr   				{ $$ = ast_expr_unop (ast_unop_not op, $2); } 
    | '+' expr %prec INT_POS_SIGN		{ $$ = ast_expr_unop (ast_unop_plus, $2); } 
    | '-' expr %prec INT_NEG_SIGN		{ $$ = ast_expr_unop (ast_unop_minus, $2); } 
    | "+." expr %prec FLOAT_POS_SIGN		{ $$ = ast_expr_unop (ast_unop_fplus, $2); } 
    | "-." expr %prec FLOAT_NEG_SIGN		{ $$ = ast_expr_unop (ast_unop_fminus, $2); } 
    | expr '+' expr 				{ $$ = ast_expr_binop($1, ast_binop_plus, $3); }
    | expr '-' expr 				{ $$ = ast_expr_binop($1, ast_binop_minus, $3); }
    | expr '*' expr 				{ $$ = ast_expr_binop($1, ast_binop_times, $3); }
    | expr '/' expr 				{ $$ = ast_expr_binop($1, ast_binop_div, $3); }
    | expr "mod" expr 				{ $$ = ast_expr_binop($1, ast_binop_mod, $3); }
    | expr "+." expr 				{ $$ = ast_expr_binop($1, ast_binop_fplus, $3); }
    | expr "-." expr 				{ $$ = ast_expr_binop($1, ast_binop_fminus, $3); }
    | expr "*." expr 				{ $$ = ast_expr_binop($1, ast_binop_ftimes, $3); }
    | expr "/." expr 				{ $$ = ast_expr_binop($1, ast_binop_fdiv, $3); }
    | expr "**" expr 				{ $$ = ast_expr_binop($1, ast_binop_exp, $3); }
    | expr '=' expr 				{ $$ = ast_expr_binop($1, ast_binop_eq, $3); }
    | expr "<>" expr 				{ $$ = ast_expr_binop($1, ast_binop_ne, $3); }
    | expr '<' expr 				{ $$ = ast_expr_binop($1, ast_binop_lt, $3); }
    | expr '>' expr 				{ $$ = ast_expr_binop($1, ast_binop_gt, $3); }
    | expr "<=" expr 				{ $$ = ast_expr_binop($1, ast_binop_le, $3); }
    | expr ">=" expr 				{ $$ = ast_expr_binop($1, ast_binop_ge, $3); }
    | expr "==" expr 				{ $$ = ast_expr_binop($1, ast_binop_pheq, $3); }
    | expr "!=" expr 				{ $$ = ast_expr_binop($1, ast_binop_phne, $3); }
    | expr "&&" expr 				{ $$ = ast_expr_binop($1, ast_binop_and, $3); }
    | expr "||" expr 				{ $$ = ast_expr_binop($1, ast_binop_or, $3); }
    | expr ":=" expr 				{ $$ = ast_expr_binop($1, ast_binop_assign, $3); }
    | expr ';' expr 				{ $$ = ast_expr_binop($1, ast_binop_semicolon, $3); }
    | let_definition "in" expr 						{ $$ = ast_expr_let($1, $3); }
    | "while" expr "do" expr "done" 					{ $$ = ast_expr_while($2, $4); }
    | "for" "id" '=' expr "to" expr "do" expr "done" 			{ $$ = ast_expr_for ($2, $4, false, $6, $8); } 
    | "for" "id" '=' expr "downto" expr "do" expr "done" 		{ $$ = ast_expr_for ($2, $4, true, $6, $8); } 
    | "dim" "id" 							{ $$ =  ast_expr_dim (NULL, $2); } 
    | "dim" "int_const" "id" 						{ $$ =  ast_expr_dim ($2, $3); } 
    | "new" type 							{ $$ = ast_expr_new($1); }
    | "delete" type 							{ $$ = ast_expr_delete($1); }
    | "id" many_expr_high
    | "constructor" many_expr_high
    | "if" expr "then" expr 						{ $$ = ast_expr_if ($2, $4, NULL); }
    | "if" expr "then" expr "else" expr 				{ $$ = ast_expr_if ($2, $4, $6); }
    | "begin" expr "end"						{ $$ = $2; }
    | "match" expr "with" clause_list "end" 				{ $$ = ast_expr_match ($2, $4); }
    | expr_high 							{ $$ = $1; }
;

%%
