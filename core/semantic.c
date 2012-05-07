/******************************************************************************
 *  CVS version:
 *     $Id: pretty.c,v 1.3 2005/04/25 12:35:11 nickie Exp $
 ******************************************************************************
 *
 *  C code file : pretty.c
 *  Project     : Llama Compiler
 *  Version     : 1.0 alpha
 *  Description : Pretty printer for abstract syntax
 *
 *  Comments: (in Greek iso-8859-7)
 *  ---------
 *  Εθνικό Μετσόβιο Πολυτεχνείο.
 *  Σχολή Ηλεκτρολόγων Μηχανικών και Μηχανικών Υπολογιστών.
 *  Τομέας Τεχνολογίας Πληροφορικής και Υπολογιστών.
 *  Εργαστήριο Τεχνολογίας Λογισμικού
 */


/* ---------------------------------------------------------------------
   ---------------------------- Header files ---------------------------
   --------------------------------------------------------------------- */

#include <string.h>
#include "general.h"
#include "error.h"
#include "ast.h"
#include "semantic.h"
#include "symbol.h"

SymbolTable symbol_table;
SymbolTable type_symbol_table;

Identifier to_hash(AST_expr e) {
    char temp[4000]; /* TODO: Fix this */

    switch (e->kind) {
        case EXPR_iconst:
            sprintf(temp, "__int_%d", e->u.e_iconst.rep);
            break;
        case EXPR_fconst:
            sprintf(temp, "__float_%lf", e->u.e_fconst.rep);
            break;
        case EXPR_cconst:
            sprintf(temp, "__float_%s", e->u.e_cconst.rep);
            break;
        case EXPR_strlit:
            sprintf(temp, "__char_%s", e->u.e_cconst.rep);
            break;
        case EXPR_true:
            strcpy(temp, "__true");
            break;
        case EXPR_false:
            strcpy(temp, "__false");
            break;
        case EXPR_unit:
            strcpy(temp, "__unit");
            break;
        default:
            internal("Can't convert expression tag to hash\n");
    }

    return id_make(temp);
}

void add_function(const char *name, Type parameter_type, Type result_type ) {
    SymbolEntry entry;

    entry = symbol_enter(symbol_table, id_make(name), 0);
    entry->entry_type = ENTRY_FUNCTION;
    entry->e.function.result_type = result_type;
    /* TODO: add parameter */
}

/* ---------------------------------------------------------------------
   ---- Implementation of functions required by the abstract syntax ----
   --------------------------------------------------------------------- */

/* void Type_print (FILE * f, int prec, Type type) */
/* { */
/*    indent(f, prec); */
/*    if (type == NULL) { */
/*       fprintf(f, "<<NULL>>\n"); */
/*       return; */
/*    } */
/*    switch (type->kind) { */
/*       case TYPE_unit: */
/*          fprintf(f, "Type: unit\n"); */
/*          break; */
/*       case TYPE_bool: */
/*          fprintf(f, "Type: bool\n"); */
/*          break; */
/*       case TYPE_char: */
/*          fprintf(f, "Type: char\n"); */
/*          break; */
/*       case TYPE_int: */
/*          fprintf(f, "Type: int\n"); */
/*          break; */
/*       case TYPE_float: */
/*          fprintf(f, "Type: float\n"); */
/*          break; */
/*       case TYPE_array: */
/*          fprintf(f, "Type: array (\n"); */
/*          indent(f, prec+1); */
/*          fprintf(f, "dim = %d\n", type->u.t_array.dim); */
/*          Type_print(f, prec+1, type->u.t_array.type); */
/*          indent(f, prec); fprintf(f, ")\n"); */
/*          break; */
/*       case TYPE_ref: */
/*          fprintf(f, "Type: ref (\n"); */
/*          Type_print(f, prec+1, type->u.t_ref.type); */
/*          indent(f, prec); fprintf(f, ")\n"); */
/*          break; */
/*       case TYPE_func: */
/*          fprintf(f, "Type: func (\n"); */
/*          Type_print(f, prec+1, type->u.t_func.type1); */
/*          Type_print(f, prec+1, type->u.t_func.type2); */
/*          indent(f, prec); fprintf(f, ")\n"); */
/*          break; */
/*       case TYPE_id: */
/*          fprintf(f, "Type: id (\n"); */
/*          Identifier_print(f, prec+1, type->u.t_id.id); */
/*          indent(f, prec); fprintf(f, ")\n"); */
/*          break; */
/*       default: */
/*          internal("invalid AST"); */
/*    } */
/* } */

void AST_program_traverse (AST_program p)
{
    if (p == NULL) {
        /* fprintf(f, "<<NULL>>\n"); */
        return;
    }

    type_symbol_table = symbol_make(193); /* TODO: find a better size */
    symbol_table = symbol_make(193); /* TODO: find a better size */
    scope_open(symbol_table);

#define ADD_FUNC(str, par, res) \
    add_function(str, type_##par(), type_##res());
#define type_STRING() \
    type_array(1, type_char())
#define type_INT_REF() \
    type_ref(type_int())

    ADD_FUNC("print_int", int, unit)
    ADD_FUNC("print_bool", bool, unit)
    ADD_FUNC("print_char", char, unit)
    ADD_FUNC("print_float", float, unit)
    ADD_FUNC("print_string", STRING, unit)

    ADD_FUNC("read_int", unit, int)
    ADD_FUNC("read_bool", unit, bool)
    ADD_FUNC("read_char", unit, char)
    ADD_FUNC("read_float", unit, float)
    ADD_FUNC("read_string", STRING, unit)

    ADD_FUNC("fabs", float, float)
    ADD_FUNC("sqrt", float, float)
    ADD_FUNC("sin", float, float)
    ADD_FUNC("cos", float, float)
    ADD_FUNC("tan", float, float)
    ADD_FUNC("atan", float, float)
    ADD_FUNC("exp", float, float)
    ADD_FUNC("ln", float, float)
    ADD_FUNC("pi", unit, float)
    
    ADD_FUNC("incr", INT_REF, unit)
    ADD_FUNC("decr", INT_REF, unit)
    
    ADD_FUNC("float_of_int", int, float)
    ADD_FUNC("int_of_float", float, int)
    ADD_FUNC("round", float, int)

    ADD_FUNC("int_of_char", char, int)
    ADD_FUNC("char_of_int", int, char)

    AST_ltdef_list_traverse(p->list);
    scope_close(symbol_table);
}

Scope AST_letdef_traverse (AST_letdef ld)
{
    Scope scope;
    if (ld == NULL) {
        return NULL;
    }

    scope = scope_open(symbol_table);

    if ( ld->recFlag == false ) {
        scope_hide(scope, true);
    }

    AST_def_list_traverse(ld->list);

    if ( ld->recFlag == false ) {
        scope_hide(scope, false);
    }

    return scope;
}

Scope AST_typedef_traverse(AST_typedef td) {
    Scope scope;

    if (td == NULL) {
        return NULL;
    }

    scope = scope_open(type_symbol_table);
    AST_tdef_list_traverse(td->list, scope);

    return scope;
}

void AST_def_traverse(AST_def d) {
    SymbolEntry entry;
    int dim_count;

    if (d == NULL) {
        /* fprintf(f, "<<NULL>>\n"); */
        return;
    }
    switch (d->kind) {
        case DEF_normal:
            entry = symbol_enter(symbol_table, d->u.d_normal.id, 0);
            entry->entry_type = ENTRY_FUNCTION;
            entry->e.function.result_type = d->u.d_normal.type;

            scope_open(symbol_table);

            AST_par_list_traverse(d->u.d_normal.list);
            AST_expr_traverse(d->u.d_normal.expr);

            scope_close(symbol_table);
            break;
        case DEF_mutable:
            dim_count = AST_expr_list_count(d->u.d_mutable.list);
            entry = symbol_enter(symbol_table, d->u.d_mutable.id, 0);
            entry->entry_type = ENTRY_VARIABLE;
            entry->e.variable.type = ( dim_count == 0 ? d->u.d_mutable.type :
                    type_array(dim_count, d->u.d_mutable.type) );
            break;
        default:
            internal("invalid AST");
    }
}

void AST_tdef_traverse(AST_tdef td, Scope scope) {
    SymbolEntry entry;
    Type user_type;

    if (td == NULL) {
        return;
    }

    user_type = type_id(td->id);

    entry = symbol_enter(type_symbol_table, td->id, 1);
    entry->entry_type = ENTRY_TYPE;
    entry->e.type.type = user_type;
    entry->e.type.scope = scope;
    AST_constr_list_traverse(td->list, user_type);
}

void AST_constr_traverse(AST_constr c, Type user_type) {
    SymbolEntry entry;

    if (c == NULL) {
        return;
    }

    /* do not allow two types to use the same constructor */
    entry = symbol_lookup(type_symbol_table, c->id, LOOKUP_ALL_SCOPES, 0);
    if ( entry != NULL )
        error("Constructor %s is used more than one times\n",  c->id->name);

    /* do not allow the same constructor to defined twice in a type definition */
    entry = symbol_enter(type_symbol_table, c->id, 1);

    entry->entry_type = ENTRY_CONSTRUCTOR;
    entry->e.constructor.type = user_type;
    Type_list_traverse(c->list);
}

void AST_par_traverse (AST_par p)
{
    SymbolEntry entry;

    if (p == NULL) {
        /* fprintf(f, "<<NULL>>\n"); */
        return;
    }

    entry = symbol_enter(symbol_table, p->id, 1);
    entry->entry_type = ENTRY_PARAMETER;
    entry->e.parameter.type = p->type;
}

Type AST_expr_traverse (AST_expr e) {
    SymbolEntry entry;
    Type expr1_type, expr2_type, expr3_type, result_type;
    Scope scope;

    if (e == NULL) {
        /* fprintf(f, "<<NULL>>\n"); */
        return NULL;
    }
    switch (e->kind) {
        case EXPR_iconst:
            /* entry = symbol_enter(symbol_table, to_hash(e), 0); */
            /* entry->entry_type = ENTRY_CONSTANT; */
            /* entry->e.constant.type = type_int(); */
            /* entry->e.constant.value.v_int = e->u.e_iconst.rep; */
            /* return entry->e.constant.type; */
            return type_int();

        case EXPR_fconst:
            /* entry = symbol_enter(symbol_table, to_hash(e), 0); */
            /* entry->entry_type = ENTRY_CONSTANT; */
            /* entry->e.constant.type = type_float(); */
            /* entry->e.constant.value.v_float = e->u.e_fconst.rep; */
            /* return entry->e.constant.type; */
            return type_float();

        case EXPR_cconst:
            /* entry = symbol_enter(symbol_table, to_hash(e), 0); */
            /* entry->entry_type = ENTRY_CONSTANT; */
            /* entry->e.constant.type = type_char(); */
            /* entry->e.constant.value.v_char = e->u.e_cconst.rep; */
            /* return entry->e.constant.type; */
            return type_char();

        case EXPR_strlit:
            /* entry = symbol_enter(symbol_table, to_hash(e), 0); */
            /* entry->entry_type = ENTRY_CONSTANT; */
            /* entry->e.constant.type = type_array(1, type_char()); */
            /* entry->e.constant.value.v_strlit = e->u.e_strlit.rep; */
            /* return entry->e.constant.type; */
            return type_array(1, type_char());

        case EXPR_true:
        case EXPR_false:
            return type_bool();

        case EXPR_unit:
            return type_unit();

        case EXPR_unop:
            expr1_type = AST_expr_traverse(e->u.e_unop.expr);
            return AST_unop_traverse(e->u.e_unop.op, expr1_type);

        case EXPR_binop:
            expr1_type = AST_expr_traverse(e->u.e_binop.expr1);
            expr2_type = AST_expr_traverse(e->u.e_binop.expr2);
            return AST_binop_traverse(expr1_type, e->u.e_binop.op, expr2_type);

        case EXPR_id: 
            entry = symbol_lookup(symbol_table, e->u.e_id.id, LOOKUP_ALL_SCOPES, 1);

            switch(entry->entry_type) {
                case ENTRY_FUNCTION:
                    /* TODO: na koitame an ontws exei 0 parametrous */
                    return entry->e.function.result_type;
                case ENTRY_PARAMETER:
                    return entry->e.parameter.type;
                case ENTRY_IDENTIFIER:
                    return entry->e.identifier.type;
                case ENTRY_VARIABLE:
                    return entry->e.variable.type;
                default:
                    error("Unknown identifier %s\n", e->u.e_id.id);
            }

        case EXPR_Id: 
            entry = symbol_lookup(type_symbol_table, e->u.e_Id.id, LOOKUP_ALL_SCOPES, 1);

            if (entry->entry_type != ENTRY_CONSTRUCTOR ) {
                internal("%s is not a constructor\n", e->u.e_Id.id);
            }

            return entry->e.constructor.type;

        case EXPR_call: 
            entry = symbol_lookup(symbol_table, e->u.e_call.id, LOOKUP_ALL_SCOPES, 1);
            /*TODO: uncomment this AST_expr_list_traverse(e->u.e_call.list); */
            return entry->e.function.result_type;
        /*case EXPR_Call: 
             
            Identifier_print(f, prec+1, e->u.e_Call.id); 
            AST_expr_list_traverse(e->u.e_Call.list); 
             
            break; 
        case EXPR_arrel: 
        
            Identifier_print(f, prec+1, e->u.e_arrel.id); 
            AST_expr_list_traverse(e->u.e_arrel.list); 
             
            break; */
        case EXPR_dim:
            entry = symbol_lookup(symbol_table, e->u.e_dim.id, LOOKUP_ALL_SCOPES, 1);
            if ( entry->entry_type != ENTRY_VARIABLE )
                error("Type mismatch: Identifier %s is not a variable\n", e->u.e_dim.id->name);
            if ( entry->e.variable.type->kind != TYPE_array )
                error("Type mismatch: %s is not an array\n", e->u.e_dim.id->name);
            return type_int();

        case EXPR_new:
            if ( e->u.e_new.type->kind == TYPE_array )
                error("Typ mismatch: New variable can't be of array type\n");
            return type_ref(e->u.e_new.type);

        case EXPR_delete:
            result_type = AST_expr_traverse(e->u.e_delete.expr);
            if ( result_type->kind != TYPE_ref )
                error("Type mismatch: Expression is not of type ref\n");
            return type_unit();

        case EXPR_let:
            scope = AST_letdef_traverse(e->u.e_let.def);
            result_type = AST_expr_traverse(e->u.e_let.expr);
            scope_close(symbol_table);
            return result_type;

        case EXPR_if: 
            expr1_type = AST_expr_traverse(e->u.e_if.econd);
            if ( expr1_type->kind != TYPE_bool )
                error("Type mismatch: Condition is not of type bool\n");
            expr2_type = AST_expr_traverse(e->u.e_if.ethen); 
            expr3_type = AST_expr_traverse(e->u.e_if.eelse);       
            if ( expr2_type->kind != expr3_type->kind )
                error("Type mismatch: Then and else parts don't match\n");
            return expr2_type;

        case EXPR_while:
            expr1_type = AST_expr_traverse(e->u.e_while.econd);
            if ( expr1_type->kind != TYPE_bool )
                error("Type mismatch: Condition is not of type bool\n");
            return AST_expr_traverse(e->u.e_while.ebody);

        case EXPR_for:  
            scope_open(symbol_table);
            entry = symbol_enter(symbol_table, e->u.e_for.id, 0);
            entry->entry_type = ENTRY_IDENTIFIER;
            entry->e.identifier.type = type_int();
            expr1_type = AST_expr_traverse(e->u.e_for.expr1);
            if ( expr1_type->kind != TYPE_int )
                error("Type mismatch: Start part of 'for' is not of type int\n");             
            expr2_type = AST_expr_traverse(e->u.e_for.expr2); 
            if ( expr2_type->kind != TYPE_int )
                error("Type mismatch: End part of 'for' is not of type int\n");
            expr3_type = AST_expr_traverse(e->u.e_for.ebody); 
            if ( expr2_type->kind != TYPE_unit )
                error("Type mismatch: Expression of 'for' is not of type unit\n");
            scope_close(symbol_table);
            return type_unit();

        case EXPR_match:
            /* TODO: type checking */
            expr1_type = AST_expr_traverse(e->u.e_match.expr);
            AST_clause_list_traverse(e->u.e_match.list);
            return NULL;

        default:
            internal("invalid AST");
    }

    return NULL;
}

void AST_clause_traverse(AST_clause c)
{
   if (c == NULL) {
      return;
   }
   /* TODO: uncomment this AST_pattern_print(c->pattern); */
   /*       ... and this AST_expr_print(c->expr); */
}

/* void AST_pattern_print (FILE * f, int prec, AST_pattern p) */
/* { */
/*    indent(f, prec); */
/*    if (p == NULL) { */
/*       fprintf(f, "<<NULL>>\n"); */
/*       return; */
/*    } */
/*    switch (p->kind) { */
/*       case PATTERN_iconst: */
/*          fprintf(f, "ast_pattern: iconst (\n"); */
/*          RepInt_print(f, prec+1, p->u.p_iconst.rep); */
/*          indent(f, prec); fprintf(f, ")\n"); */
/*          break; */
/*       case PATTERN_fconst: */
/*          fprintf(f, "ast_pattern: fconst (\n"); */
/*          RepFloat_print(f, prec+1, p->u.p_fconst.rep); */
/*          indent(f, prec); fprintf(f, ")\n"); */
/*          break; */
/*       case PATTERN_cconst: */
/*          fprintf(f, "ast_pattern: cconst (\n"); */
/*          RepChar_print(f, prec+1, p->u.p_cconst.rep); */
/*          indent(f, prec); fprintf(f, ")\n"); */
/*          break; */
/*       case PATTERN_true: */
/*          fprintf(f, "ast_pattern: true\n"); */
/*          break; */
/*       case PATTERN_false: */
/*          fprintf(f, "ast_pattern: false\n"); */
/*          break; */
/*       case PATTERN_id: */
/*          fprintf(f, "ast_pattern: id (\n"); */
/*          Identifier_print(f, prec+1, p->u.p_id.id); */
/*          indent(f, prec); fprintf(f, ")\n"); */
/*          break; */
/*       case PATTERN_Id: */
/*          fprintf(f, "ast_pattern: Id (\n"); */
/*          Identifier_print(f, prec+1, p->u.p_Id.id); */
/*          AST_pattern_list_print(f, prec+1, p->u.p_Id.list); */
/*          indent(f, prec); fprintf(f, ")\n"); */
/*          break; */
/*       default: */
/*          internal("invalid AST"); */
/*    } */
/* } */

Type AST_unop_traverse(AST_unop op, Type expr) {
    switch (op) {
        case ast_unop_plus:
        case ast_unop_minus:
            if ( expr->kind != TYPE_int )
                error("Type mismatch: Argument is not of type int\n");
            return type_int();

        case ast_unop_fplus:
        case ast_unop_fminus:
            if ( expr->kind != TYPE_float )
                error("Type mismatch: Argument is not of type float\n");
            return type_float();

        case ast_unop_exclam:
            if ( expr->kind != TYPE_ref )
                error("Type mismatch: Argument is not of type ref\n");
            return expr->u.t_ref.type;

        case ast_unop_not:
            if ( expr->kind != TYPE_bool )
                error("Type mismatch: Argument is not of type bool\n");
            return type_bool();

        default:
            internal("invalid AST");
    }

    return NULL;
}

Type AST_binop_traverse(Type expr1, AST_binop op, Type expr2) {
    switch (op) {
        case ast_binop_plus:
        case ast_binop_minus:
        case ast_binop_times:
        case ast_binop_div:
        case ast_binop_mod:
            if ( expr1->kind != TYPE_int )
                error("Type mismatch in the left argument\n");
            if ( expr2->kind != TYPE_int )
                error("Type mismatch in the right argument\n");
            return type_int();

        case ast_binop_fplus:
        case ast_binop_fminus:
        case ast_binop_ftimes:
        case ast_binop_fdiv:
        case ast_binop_exp:
            if ( expr1->kind != TYPE_float )
                error("Type mismatch in the left argument\n");
            if ( expr2->kind != TYPE_float )
                error("Type mismatch in the right argument\n");
            return type_float();

        case ast_binop_lt:
        case ast_binop_gt:
        case ast_binop_le:
        case ast_binop_ge:
            if ( expr1->kind != expr2->kind )
                error("Type mismatch: Arguments must be of the same type\n");
            if ( expr1->kind != TYPE_char && expr1->kind != TYPE_float && expr1->kind != TYPE_int )
                error("Type mismatch: Arguments must be of type char, float or int\n");
            return type_bool();

        case ast_binop_eq:
        case ast_binop_ne:
        case ast_binop_pheq:
        case ast_binop_phne:
            if ( expr1->kind != expr2->kind )
                error("Type mismatch: Arguments must be of the same type\n");
            if ( expr1->kind == TYPE_array )
                error("Type mismatch: Arguments can't be of type array\n");
            if ( expr1->kind == TYPE_func )
                error("Type mismatch: Arguments can't be of type function\n");
            return type_bool();

        case ast_binop_and:
        case ast_binop_or:
            if ( expr1->kind != TYPE_bool )
                error("Type mismatch in the left argument\n");
            if ( expr2->kind != TYPE_bool )
                error("Type mismatch in the right argument\n");
            return type_bool();

        case ast_binop_semicolon:
            return expr2;

        case ast_binop_assign:
            if ( expr1->kind != TYPE_ref ) 
                error("Type mismatch: First argument must be of type ref\n");
            else if ( expr1->u.t_ref.type->kind != expr2->kind )
                error("Type mismatch: First argument must be of type ref\n"); /* TODO: fix this message */
            return type_unit();

        default:
            internal("invalid AST");
    }

    return NULL;
}

void AST_ltdef_list_traverse (AST_ltdef_list l)
{
    if (l == NULL) {
        /* fprintf(f, "<<NULL>>\n"); */
        return;
    }
    switch (l->kind) { /* TODO: xreiazetai ontws na anoiksoume scope edw? (MALLON OXI) */
        case LTDEF_let:
            AST_letdef_traverse(l->head.letdef);
            AST_ltdef_list_traverse(l->tail);
            break;
        case LTDEF_type:
            AST_typedef_traverse(l->head.typdef);
            AST_ltdef_list_traverse(l->tail);
            break;
        default:
            internal("invalid AST");
    }
}

void AST_def_list_traverse (AST_def_list l)
{
    if (l == NULL) return;

    AST_def_traverse(l->head);
    AST_def_list_traverse(l->tail);
}

void AST_tdef_list_traverse(AST_tdef_list l, Scope scope)
{
    if (l == NULL) return;

    AST_tdef_traverse(l->head, scope);
    AST_tdef_list_traverse(l->tail, scope);
}

void AST_constr_list_traverse(AST_constr_list l, Type user_type)
{
    if (l == NULL) return;

    AST_constr_traverse(l->head, user_type);
    AST_constr_list_traverse(l->tail, user_type);
}

void AST_par_list_traverse(AST_par_list l)
{
    if (l == NULL) {
        /*fprintf(f, "<<NULL>>\n");*/
        return;
    }
    AST_par_traverse(l->head);
    AST_par_list_traverse(l->tail);
}

int AST_expr_list_count(AST_expr_list l) {
   if (l == NULL) return 0;

   return 1 + AST_expr_list_count(l->tail);
}

/* void AST_expr_list_print (FILE * f, int prec, AST_expr_list l) */
/* { */
/*    indent(f, prec); */
/*    if (l == NULL) { */
/*       fprintf(f, "<<NULL>>\n"); */
/*       return; */
/*    } */
/*    fprintf(f, "ast_expr_list (\n"); */
/*    AST_expr_print(f, prec+1, l->head); */
/*    AST_expr_list_print(f, prec+1, l->tail); */
/*    indent(f, prec); fprintf(f, ")\n"); */
/* } */

void AST_clause_list_traverse(AST_clause_list l)
{
   if (l == NULL) {
      return;
   }
   AST_clause_traverse(l->head);
   AST_clause_list_traverse(l->tail);
}

/* void AST_pattern_list_print (FILE * f, int prec, AST_pattern_list l) */
/* { */
/*    indent(f, prec); */
/*    if (l == NULL) { */
/*       fprintf(f, "<<NULL>>\n"); */
/*       return; */
/*    } */
/*    fprintf(f, "ast_par_list (\n"); */
/*    AST_pattern_print(f, prec+1, l->head); */
/*    AST_pattern_list_print(f, prec+1, l->tail); */
/*    indent(f, prec); fprintf(f, ")\n"); */
/* } */
/*  */
void Type_list_traverse(Type_list l) {
    if (l == NULL) {
        return;
    }

    /* TODO: mipws den epitrepontai kapoioi typoi? (p.x. ref) */
    switch(l->head->kind) {
        case TYPE_id:
            symbol_lookup(type_symbol_table, l->head->u.t_id.id, LOOKUP_ALL_SCOPES, 1);
            break;

        default:
            break;
    }
    Type_list_traverse(l->tail);
}
