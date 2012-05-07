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
#include "pretty.h"

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
    entry->e.function.type = type_func(parameter_type, result_type);
}

Type unify(Type first_type, Type second_type, bool err) {
    if ( first_type == NULL ) first_type = type_unknown();
    if ( second_type == NULL ) second_type = type_unknown();

    if ( first_type->kind == TYPE_unknown &&
            second_type->kind == TYPE_unknown ) return type_unknown();

    if ( first_type->kind == TYPE_unknown ) return second_type;
    if ( second_type->kind == TYPE_unknown ) return first_type;

    if ( first_type->kind == second_type->kind ) return first_type;

    error("Cannot unify types\n");

    return type_unknown();
}

bool type_check_ref(Type a, bool deft) {
    if ( a == NULL ) return deft;
    return a->kind == TYPE_ref;
}

bool type_check_array(Type a, bool deft) {
    if ( a == NULL ) return deft;
    return a->kind == TYPE_array;
}

bool type_eq(Type a, Type b) {
    if ( a == NULL || b == NULL ) return true;
    if ( a->kind == TYPE_unknown || b->kind == TYPE_unknown ) return true;
    if ( a->kind != b->kind ) return false;

    switch(a->kind) {
        case TYPE_func:
            return type_eq(a->u.t_func.type1, b->u.t_func.type1) &&
                type_eq(a->u.t_func.type2, b->u.t_func.type2);
        case TYPE_id:
            return strcmp(a->u.t_id.id->name, b->u.t_id.id->name) == 0;
        case TYPE_ref:
            return type_eq(a->u.t_ref.type, b->u.t_ref.type);
        case TYPE_array:
            return type_eq(a->u.t_array.type, b->u.t_array.type) &&
                a->u.t_array.dim == b->u.t_array.dim;
        default:
            return true;
    }
}

/* ---------------------------------------------------------------------
   ---- Implementation of functions required by the abstract syntax ----
   --------------------------------------------------------------------- */

void AST_program_traverse(AST_program p) {
    if (p == NULL) {
        return;
    }

    type_symbol_table = symbol_make(28447);
    symbol_table = symbol_make(28447);
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

    ADD_FUNC("strlen", char, int)
    add_function("strcmp", type_func(type_STRING(), type_STRING()), type_int());
    add_function("strcpy", type_func(type_STRING(), type_STRING()), type_unit());
    add_function("strcat", type_func(type_STRING(), type_STRING()), type_unit());

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
    Type expr_type,/* unif_type,*/ par_type;
    int dim_count;

    if (d == NULL) return;

    switch (d->kind) {
        case DEF_normal:
            entry = symbol_enter(symbol_table, d->u.d_normal.id, 0);
            entry->entry_type = ENTRY_FUNCTION;
            entry->e.function.result_type = d->u.d_normal.type;

            scope_open(symbol_table);

            par_type = AST_par_list_traverse(d->u.d_normal.list);
            /* warning("SETTING %s\n", d->u.d_normal.id->name);  */
            /* Type_print(stdout, 0, par_type); */
            if ( d->u.d_normal.list == NULL )
                entry->e.function.type = d->u.d_normal.type;
            else
                entry->e.function.type = type_func(par_type, d->u.d_normal.type);

            expr_type = AST_expr_traverse(d->u.d_normal.expr);

            if ( !type_eq(expr_type, entry->e.function.result_type) )
                error("Function expression type does not match its definition\n");
            /* entry->e.function.result_type = unify(expr_type, d->u.d_normal.type, 1); */

            scope_close(symbol_table);
            break;
        case DEF_mutable:
            dim_count = AST_expr_list_count(d->u.d_mutable.list);
            entry = symbol_enter(symbol_table, d->u.d_mutable.id, 0);
            entry->entry_type = ENTRY_VARIABLE;
            entry->e.variable.type = ( dim_count == 0 ?
                    type_ref(d->u.d_mutable.type) :
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
        error("Constructor %s is used more than one time\n",  c->id->name);

    /* do not allow the same constructor to defined twice in a type definition */
    entry = symbol_enter(type_symbol_table, c->id, 1);

    entry->entry_type = ENTRY_CONSTRUCTOR;
    entry->e.constructor.type = user_type;
    entry->e.constructor.argument_type = Type_list_traverse(c->list);
}

Type AST_par_traverse(AST_par p)
{
    SymbolEntry entry;

    if (p == NULL) return NULL;

    entry = symbol_enter(symbol_table, p->id, 1);
    entry->entry_type = ENTRY_PARAMETER;
    entry->e.parameter.type = p->type;

    return p->type;
}

Type AST_expr_traverse(AST_expr e) {
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
                    return entry->e.function.type;
                case ENTRY_PARAMETER:
                    return entry->e.parameter.type;
                case ENTRY_IDENTIFIER:
                    return entry->e.identifier.type;
                case ENTRY_VARIABLE:
                    return entry->e.variable.type;
                default:
                    error("Unknown identifier %s\n", e->u.e_id.id->name);
            }

        case EXPR_Id: 
            entry = symbol_lookup(type_symbol_table, e->u.e_Id.id, LOOKUP_ALL_SCOPES, 1);

            if (entry->entry_type != ENTRY_CONSTRUCTOR ) {
                error("'%s' is not a constructor\n", e->u.e_Id.id->name);
            }

            return entry->e.constructor.type;

        case EXPR_call: 
            /* warning("Calling %s...\n", e->u.e_call.id->name); */
            entry = symbol_lookup(symbol_table, e->u.e_call.id, LOOKUP_ALL_SCOPES, 1);

            if  ( entry->entry_type != ENTRY_FUNCTION )
                error("'%s' is not a function\n", e->u.e_call.id->name);

            expr1_type = AST_expr_list_traverse(e->u.e_call.list);

            if ( !type_eq(expr1_type, entry->e.function.type->u.t_func.type1) ) {
                error("The types of the arguments of function '%s' do not match the definition\n", e->u.e_call.id->name);
            }

            /* warning("Finished %s...\n", e->u.e_call.id->name); */
            return entry->e.function.result_type;

        case EXPR_Call: 
            entry = symbol_lookup(type_symbol_table, e->u.e_Call.id, LOOKUP_ALL_SCOPES, 1);

            if ( entry->entry_type != ENTRY_CONSTRUCTOR ) {
                error("'%s' is not a constructor\n", e->u.e_Call.id->name);
            }
            expr1_type = AST_expr_list_traverse(e->u.e_Call.list); 
            if ( !type_eq(expr1_type, entry->e.constructor.argument_type) ) {
                error("The types of the arguments of the constructor '%s' do not match the definition\n", e->u.e_Call.id->name);
            }

            return entry->e.constructor.type;

        case EXPR_arrel: 
            entry = symbol_lookup(symbol_table, e->u.e_arrel.id, LOOKUP_ALL_SCOPES, 1);
            AST_expr_list_traverse(e->u.e_arrel.list); 

            switch(entry->entry_type) {
                case ENTRY_VARIABLE:
                    if ( entry->e.variable.type->kind != TYPE_array )
                        error("Type mismatch: '%s' is not an array\n", e->u.e_arrel.id->name);

                    return entry->e.variable.type->u.t_array.type;
                case ENTRY_PARAMETER:
                    return entry->e.parameter.type; /* TODO: fix this */
                default:
                    error("Type mismatch: '%s' is not an array\n", e->u.e_dim.id->name);
            }

        case EXPR_dim:
            entry = symbol_lookup(symbol_table, e->u.e_dim.id, LOOKUP_ALL_SCOPES, 1);
            switch(entry->entry_type) {
                case ENTRY_VARIABLE:
                    if ( entry->e.variable.type->kind != TYPE_array )
                        error("Type mismatch: '%s' is not an array\n", e->u.e_dim.id->name);
                    break;
                case ENTRY_PARAMETER:
                    break;
                default:
                    error("Type mismatch: '%s' is not a valid argument for operator 'dim'\n", e->u.e_dim.id->name);
            }

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
            if ( !type_eq(expr1_type, type_bool()) )
                error("Type mismatch: Condition is not of type bool\n");
            expr2_type = AST_expr_traverse(e->u.e_if.ethen); 
            expr3_type = AST_expr_traverse(e->u.e_if.eelse);       
            if ( !type_eq(expr2_type, expr3_type) )
                error("Type mismatch: Then and else parts don't match\n");
            return expr2_type;

        case EXPR_while:
            expr1_type = AST_expr_traverse(e->u.e_while.econd);
            if ( !type_eq(expr1_type, type_bool()) )
                error("Type mismatch: Condition is not of type bool\n");
            return AST_expr_traverse(e->u.e_while.ebody);

        case EXPR_for:  
            scope_open(symbol_table);
            entry = symbol_enter(symbol_table, e->u.e_for.id, 0);
            entry->entry_type = ENTRY_IDENTIFIER;
            entry->e.identifier.type = type_int();
            expr1_type = AST_expr_traverse(e->u.e_for.expr1);
            if ( !type_eq(expr1_type, type_int()) )
                error("Type mismatch: Start part of 'for' is not of type int\n");             
            expr2_type = AST_expr_traverse(e->u.e_for.expr2); 
            if ( !type_eq(expr2_type, type_int()) )
                error("Type mismatch: End part of 'for' is not of type int\n");
            expr3_type = AST_expr_traverse(e->u.e_for.ebody); 
            if ( !type_eq(expr3_type, type_unit()) )
                error("Type mismatch: Expression of 'for' is not of type unit\n");
            scope_close(symbol_table);
            return type_unit();

        case EXPR_match:
            expr1_type = AST_expr_traverse(e->u.e_match.expr);
            AST_clause_list_traverse(e->u.e_match.list, expr1_type);
            return NULL;

        default:
            internal("invalid AST");
    }

    return type_unknown();
}

void AST_clause_traverse(AST_clause c, Type type) {
    if (c == NULL) {
        return;
    }
    AST_pattern_traverse(c->pattern, type);
    /*       ... and this AST_expr_print(c->expr); */
}

void AST_pattern_traverse(AST_pattern p, Type type) {
    SymbolEntry entry;

    if (p == NULL) return;

    switch (p->kind) {
        case PATTERN_iconst:
            if ( !type_eq(type, type_int()) )
                error("Type mismatch: Int pattern found, but type is not int\n");
            break;

        case PATTERN_fconst:
            if ( !type_eq(type, type_float()) )
                error("Type mismatch: Float pattern found, but type is not float\n");
            break;

        case PATTERN_cconst:
            if ( !type_eq(type, type_float()) )
                error("Type mismatch: Char pattern found, but type is not char\n");
            break;

        case PATTERN_true:
            if ( !type_eq(type, type_bool()) )
                error("Type mismatch: Pattern 'true' found, but type is not bool\n");
            break;

        case PATTERN_false:
            if ( !type_eq(type, type_bool()) )
                error("Type mismatch: Pattern 'false' found, but type is not bool\n");
            break;

        case PATTERN_id:
            break;

        case PATTERN_Id:
            entry = symbol_lookup(type_symbol_table, p->u.p_Id.id, LOOKUP_ALL_SCOPES, 1);
            if ( !type_eq(type, entry->e.constructor.type) )
                error("Type mismatch: Constructor in pattern does not match the type of the expression\n");

            type = entry->e.constructor.argument_type;
            AST_pattern_list_traverse(p->u.p_Id.list, type);

            break;

        default:
            internal("invalid AST");
    }
}

Type AST_unop_traverse(AST_unop op, Type expr) {
    switch (op) {
        case ast_unop_plus:
        case ast_unop_minus:
            if ( !type_eq(expr, type_int()) )
                error("Type mismatch: Argument is not of type int\n");
            return type_int();

        case ast_unop_fplus:
        case ast_unop_fminus:
            if ( !type_eq(expr, type_float()) )
                error("Type mismatch: Argument is not of type float\n");
            return type_float();

        case ast_unop_exclam:
            if ( !type_check_ref(expr, true) )
                error("Type mismatch: Argument is not of type ref\n");
            return (expr == NULL ? NULL : expr->u.t_ref.type);

        case ast_unop_not:
            if ( !type_eq(expr, type_bool()) )
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
            if ( !type_eq(expr1, type_int()) )
                error("Type mismatch in the left argument\n");
            if ( !type_eq(expr2, type_int()) ) {
                error("Type mismatch in the right argument\n");
            }
            return type_int();

        case ast_binop_fplus:
        case ast_binop_fminus:
        case ast_binop_ftimes:
        case ast_binop_fdiv:
        case ast_binop_exp:
            if ( !type_eq(expr1, type_float()) )
                error("Type mismatch in the left argument\n");
            if ( !type_eq(expr1, type_float()) )
                error("Type mismatch in the right argument\n");
            return type_float();

        case ast_binop_lt:
        case ast_binop_gt:
        case ast_binop_le:
        case ast_binop_ge:
            if ( !type_eq(expr1, expr2) )
                error("Type mismatch: Arguments must be of the same type\n");
            if ( !type_eq(expr1, type_char()) && !type_eq(expr1, type_float()) &&
                    !type_eq(expr1, type_int()) )
                error("Type mismatch: Arguments must be of type char, float or int\n");
            return type_bool();

        case ast_binop_eq:
        case ast_binop_ne:
        case ast_binop_pheq:
        case ast_binop_phne:
            if ( !type_eq(expr1, expr2) )
                error("Type mismatch: Arguments must be of the same type\n");
            if ( type_check_array(expr1, false) || type_check_array(expr2, false) )
                error("Type mismatch: Arguments can't be of type array\n");
            if ( type_check_ref(expr1, false) || type_check_ref(expr2, false) )
                error("Type mismatch: Arguments can't be of type function\n");
            return type_bool();

        case ast_binop_and:
        case ast_binop_or:
            if ( !type_eq(expr1, type_bool()) )
                error("Type mismatch in the left argument\n");
            if ( !type_eq(expr2, type_bool()) )
                error("Type mismatch in the right argument\n");
            return type_bool();

        case ast_binop_semicolon:
            return expr2;

        case ast_binop_assign:
            if ( !type_check_ref(expr1,true) ) 
                error("Type mismatch: First argument must be of type ref\n");
            else if ( expr1 != NULL && !type_eq(expr1->u.t_ref.type, expr2) )
                error("Type mismatch: The arguments of the assignment operator do not match\n");
            return type_unit();

        default:
            internal("invalid AST");
    }

    return NULL;
}

void AST_ltdef_list_traverse (AST_ltdef_list l)
{
    if (l == NULL) return;

    switch (l->kind) {
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

Type AST_par_list_traverse(AST_par_list l)
{
    Type temp, temp2;

    if (l == NULL) return NULL;

    temp = AST_par_traverse(l->head);
    temp2 = AST_par_list_traverse(l->tail);

    if ( l->tail != NULL )
        return type_func(temp, temp2);
    else
        return temp;
}

int AST_expr_list_count(AST_expr_list l) {
    if (l == NULL) return 0;

    return 1 + AST_expr_list_count(l->tail);
}

Type AST_expr_list_traverse(AST_expr_list l) {
    Type temp, temp2;

    if (l == NULL) return NULL;

    temp = AST_expr_traverse(l->head);
    temp2 =AST_expr_list_traverse(l->tail);

    if ( l->tail != NULL )
        return type_func(temp, temp2);
    else
        return temp;
}

void AST_clause_list_traverse(AST_clause_list l, Type type)
{
    if (l == NULL) return;

    AST_clause_traverse(l->head, type);
    AST_clause_list_traverse(l->tail, type);
}

void AST_pattern_list_traverse(AST_pattern_list l, Type type) {
    if (l == NULL) {
        if ( type != NULL )
            error ("The number of arguments given to the constructor does not match the type definition\n");
        return;
    }
    else if ( type == NULL ) {
        error ("The number of arguments given to the constructor does not match the type definition\n");
        return;
    }

    if ( type->kind == TYPE_func ) {
        AST_pattern_traverse(l->head, type->u.t_func.type1);
        AST_pattern_list_traverse(l->tail, type->u.t_func.type2);
    }
    else {
        AST_pattern_traverse(l->head, type);
        AST_pattern_list_traverse(l->tail, NULL);
    }
}

Type Type_list_traverse(Type_list l) {
    Type temp, temp2;

    if (l == NULL) return NULL;

    /* TODO: mipws den epitrepontai kapoioi typoi? (p.x. ref) */
    switch(l->head->kind) {
        case TYPE_id:
            symbol_lookup(type_symbol_table, l->head->u.t_id.id, LOOKUP_ALL_SCOPES, 1);
            break;

        default:
            break;
    }

    temp = l->head;
    temp2 = Type_list_traverse(l->tail);

    if ( temp2 == NULL ) return temp;
    return type_func(temp, temp2);
}
