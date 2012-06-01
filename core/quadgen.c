#include <string.h>
#include "general.h"
#include "error.h"
#include "ast.h"
#include "symbol.h"
#include "quadgen.h"
#include "pretty.h"

/* SymbolTable symbol_table; */
/* SymbolTable type_symbol_table; */
int global_count;
int quad_count;
Quad_list q;

/* void add_function(const char *name, Type parameter_type, Type result_type ) { */
/*     SymbolEntry entry; */
/*  */
/*     entry = symbol_enter(symbol_table, id_make(name), 0); */
/*     entry->entry_type = ENTRY_FUNCTION; */
/*     entry->e.function.result_type = result_type; */
/*     entry->e.function.type = type_func(parameter_type, result_type); */
/* } */

/* TODO: do we need to pass more information about the function? */
Function make_function(Identifier id) {
    Function x;
    x.id = id;
    return x;
}

Quad quad_append_new(Quad_opname opname, Quad_operand op1, Quad_operand op2, Quad_operand op3) {
    Quad newquad = quad(++quad_count, opname, op1, op2, op3);
    q = quad_list(newquad, q);
    return newquad;
}

void quad_backpatch(Quad quad) {
    quad->op3 = quad_operand_quadnumber(quad_count+1);
}

/* ---------------------------------------------------------------------
   ---- Implementation of functions required by the abstract syntax ----
   --------------------------------------------------------------------- */

void AST_program_quad_generate(AST_program p) {
    if (p == NULL) {
        return;
    }

    /* type_symbol_table = symbol_make(28447); */
    /* symbol_table = symbol_make(28447); */
    /* scope_open(symbol_table); */

    AST_ltdef_list_quad_generate(p->list);
    /* scope_close(symbol_table); */
}

Scope AST_letdef_quad_generate(AST_letdef ld)
{
    Scope scope;
    if (ld == NULL) {
        return NULL;
    }

    /* scope = scope_open(symbol_table); */

    /* if ( ld->recFlag == false ) { */
    /*     scope_hide(scope, true); */
    /* } */

    AST_def_list_quad_generate(ld->list);

    /* if ( ld->recFlag == false ) { */
    /*     scope_hide(scope, false); */
    /* } */

    /* return scope; */
}

Scope AST_typedef_quad_generate(AST_typedef td) {
    Scope scope;

    if (td == NULL) {
        return NULL;
    }

    /* scope = scope_open(type_symbol_table); */
    AST_tdef_list_quad_generate(td->list, scope);

    return scope;
}

void AST_def_quad_generate(AST_def d) {
    SymbolEntry entry;
    Type par_type;
    int dim_count;

    if (d == NULL) return;

    switch (d->kind) {
        case DEF_normal:
            /* entry = symbol_enter(symbol_table, d->u.d_normal.id, 0); */
            /* entry->entry_type = ENTRY_FUNCTION; */
            /* entry->e.function.result_type = d->u.d_normal.type; */

            quad_append_new(quad_opcode_unit, quad_operand_simple(quad_function(make_function(d->u.d_normal.id))), quad_operand_empty(), quad_operand_empty());
            /* scope_open(symbol_table); */

            par_type = AST_par_list_quad_generate(d->u.d_normal.list);
            /* if ( d->u.d_normal.list == NULL ) */
            /*     entry->e.function.type = d->u.d_normal.type; */
            /* else */
            /*     entry->e.function.type = type_func(par_type, d->u.d_normal.type); */

            AST_expr_quad_generate(d->u.d_normal.expr);

            /* TODO: backpatch this quad in the expression body (papaspyrou, page 198)? */
            quad_append_new(quad_opcode_endu, quad_operand_simple(quad_function(make_function(d->u.d_normal.id))), quad_operand_empty(), quad_operand_empty());

            /* scope_close(symbol_table); */
            break;
        case DEF_mutable: /* TODO */
            dim_count = AST_expr_list_count(d->u.d_mutable.list);
            /* entry = symbol_enter(symbol_table, d->u.d_mutable.id, 0); */
            /* entry->entry_type = ENTRY_VARIABLE; */
            /* entry->e.variable.type = ( dim_count == 0 ? */
            /*         type_ref(d->u.d_mutable.type) : */
            /*         type_array(dim_count, d->u.d_mutable.type) ); */
            break;
        default:
            internal("invalid AST");
    }
}

void AST_tdef_quad_generate(AST_tdef td, Scope scope) {
    SymbolEntry entry;
    Type user_type;

    if (td == NULL) {
        return;
    }

    user_type = type_id(td->id);

    /* entry = symbol_enter(type_symbol_table, td->id, 1); */
    /* entry->entry_type = ENTRY_TYPE; */
    /* entry->e.type.type = user_type; */
    /* entry->e.type.scope = scope; */
    AST_constr_list_quad_generate(td->list, user_type);
}

void AST_constr_quad_generate(AST_constr c, Type user_type) {
    /* SymbolEntry entry; */

    /* if (c == NULL) { */
    /*     return; */
    /* } */

    /* #<{(| do not allow two types to use the same constructor |)}># */
    /* entry = symbol_lookup(type_symbol_table, c->id, LOOKUP_ALL_SCOPES, 0); */
    /* if ( entry != NULL ) */
    /*     error("Constructor %s is used more than one time\n",  c->id->name); */

    /* #<{(| do not allow the same constructor to defined twice in a type definition |)}># */
    /* entry = symbol_enter(type_symbol_table, c->id, 1); */

    /* entry->entry_type = ENTRY_CONSTRUCTOR; */
    /* entry->e.constructor.type = user_type; */
    /* entry->e.constructor.argument_type = Type_list_quad_generate(c->list); */
}

Type AST_par_quad_generate(AST_par p)
{
    /* SymbolEntry entry; */

    /* if (p == NULL) return NULL; */

    /* entry = symbol_enter(symbol_table, p->id, 1); */
    /* entry->entry_type = ENTRY_PARAMETER; */
    /* entry->e.parameter.type = p->type; */

    /* return p->type; */
}

Quad_operand AST_expr_quad_generate(AST_expr e) {
    SymbolEntry entry;
    Quad_operand op1, op2, reserved, if_operand, else_operand;
    /* Type expr1_type, expr2_type, expr3_type, result_type; */
    Scope scope;
    Quad jump_to_if, jump_after_if, jump_after_else;
    Temporary res;

    if (e == NULL) {
        /* fprintf(f, "<<NULL>>\n"); */
        return NULL;
    }
    switch (e->kind) {
        case EXPR_iconst:
            return quad_operand_simple(quad_iconst(e->u.e_iconst.rep));

        case EXPR_fconst:
            return quad_operand_simple(quad_fconst(e->u.e_fconst.rep));

        case EXPR_cconst:
            return quad_operand_simple(quad_cconst(e->u.e_cconst.rep));
 
        case EXPR_strlit:
            return quad_operand_simple(quad_strlit(e->u.e_strlit.rep));
       
        case EXPR_true:
            return quad_operand_simple(quad_true());
        
        case EXPR_false:
            return quad_operand_simple(quad_false());

        case EXPR_unit:
            return quad_operand_empty();
        
        case EXPR_unop:
            op1 = AST_expr_quad_generate(e->u.e_unop.expr);
            return AST_unop_quad_generate(e->u.e_unop.op, op1);

        case EXPR_binop:
            op1 = AST_expr_quad_generate(e->u.e_binop.expr1);
            op2 = AST_expr_quad_generate(e->u.e_binop.expr2);

            return AST_binop_quad_generate(op1, e, op2);

        case EXPR_id:
            /* entry = symbol_lookup(symbol_table, e->u.e_id.id, LOOKUP_ALL_SCOPES, 1); */

            /* switch(entry->entry_type) { */
            /*     case ENTRY_FUNCTION: */
            /*         return entry->e.function.type; */
            /*     case ENTRY_PARAMETER: */
            /*         return entry->e.parameter.type; */
            /*     case ENTRY_IDENTIFIER: */
            /*         return entry->e.identifier.type; */
            /*     case ENTRY_VARIABLE: */
            /*         return entry->e.variable.type; */
            /*     default: */
            /*         error("Unknown identifier %s\n", e->u.e_id.id->name); */
            /* } */
            return quad_operand_empty(); /* TODO */

        case EXPR_Id: 
            /* entry = symbol_lookup(type_symbol_table, e->u.e_Id.id, LOOKUP_ALL_SCOPES, 1); */

            /* if (entry->entry_type != ENTRY_CONSTRUCTOR ) { */
            /*     error("'%s' is not a constructor\n", e->u.e_Id.id->name); */
            /* } */

            /* return entry->e.constructor.type; */
            return quad_operand_empty(); /* TODO */

        case EXPR_call: 
            /* entry = symbol_lookup(symbol_table, e->u.e_call.id, LOOKUP_ALL_SCOPES, 1); */

            /* if  ( entry->entry_type != ENTRY_FUNCTION ) */
            /*     error("'%s' is not a function\n", e->u.e_call.id->name); */

            /* expr1_type = AST_expr_list_quad_generate(e->u.e_call.list); */

            /* if ( !type_eq(expr1_type, entry->e.function.type->u.t_func.type1) ) { */
            /*     error("The types of the arguments of function '%s' do not match the definition\n", e->u.e_call.id->name); */
            /* } */

            /* return entry->e.function.result_type; */
            return quad_operand_empty(); /* TODO */

        case EXPR_Call: 
            /* entry = symbol_lookup(type_symbol_table, e->u.e_Call.id, LOOKUP_ALL_SCOPES, 1); */

            /* if ( entry->entry_type != ENTRY_CONSTRUCTOR ) { */
            /*     error("'%s' is not a constructor\n", e->u.e_Call.id->name); */
            /* } */
            /* expr1_type = AST_expr_list_quad_generate(e->u.e_Call.list);  */
            /* if ( !type_eq(expr1_type, entry->e.constructor.argument_type) ) { */
            /*     error("The types of the arguments of the constructor '%s' do not match the definition\n", e->u.e_Call.id->name); */
            /* } */

            /* return entry->e.constructor.type; */
            return quad_operand_empty(); /* TODO */

        case EXPR_arrel: 
            /* entry = symbol_lookup(symbol_table, e->u.e_arrel.id, LOOKUP_ALL_SCOPES, 1); */
            /* AST_expr_list_quad_generate(e->u.e_arrel.list);  */

            /* switch(entry->entry_type) { */
            /*     case ENTRY_VARIABLE: */
            /*         if ( entry->e.variable.type->kind != TYPE_array ) */
            /*             error("Type mismatch: '%s' is not an array\n", e->u.e_arrel.id->name); */

            /*         return entry->e.variable.type->u.t_array.type; */
            /*     case ENTRY_PARAMETER: */
            /*         return entry->e.parameter.type; #<{(| TODO: fix this |)}># */
            /*     default: */
            /*         error("Type mismatch: '%s' is not an array\n", e->u.e_dim.id->name); */
            /* } */
            return quad_operand_empty(); /* TODO */

        case EXPR_dim:
            /* entry = symbol_lookup(symbol_table, e->u.e_dim.id, LOOKUP_ALL_SCOPES, 1); */
            /* switch(entry->entry_type) { */
            /*     case ENTRY_VARIABLE: */
            /*         if ( entry->e.variable.type->kind != TYPE_array ) */
            /*             error("Type mismatch: '%s' is not an array\n", e->u.e_dim.id->name); */
            /*         break; */
            /*     case ENTRY_PARAMETER: */
            /*         break; */
            /*     default: */
            /*         error("Type mismatch: '%s' is not a valid argument for operator 'dim'\n", e->u.e_dim.id->name); */
            /* } */

            /* return type_int(); */
            return quad_operand_empty(); /* TODO */

        case EXPR_new:
            /* if ( e->u.e_new.type->kind == TYPE_array ) */
            /*     error("Typ mismatch: New variable can't be of array type\n"); */
            /* return type_ref(e->u.e_new.type); */
            return quad_operand_empty(); /* TODO */


        case EXPR_delete:
            /* result_type = AST_expr_quad_generate(e->u.e_delete.expr); */
            /* if ( result_type->kind != TYPE_ref ) */
            /*     error("Type mismatch: Expression is not of type ref\n"); */
            /* return type_unit(); */
            return quad_operand_empty(); /* TODO */

        case EXPR_let:
            /* scope = AST_letdef_quad_generate(e->u.e_let.def); */
            /* result_type = AST_expr_quad_generate(e->u.e_let.expr); */
            /* scope_close(symbol_table); */
            /* return result_type; */
            return quad_operand_empty(); /* TODO */

        case EXPR_if:
            /* generate condition quads */
            op1 = AST_expr_quad_generate(e->u.e_if.econd);

            jump_to_if    = quad_append_new(quad_opcode_eq, op1,
                    quad_operand_simple(quad_true()), quad_operand_empty());
            jump_after_if = quad_append_new(quad_opcode_jump, quad_operand_empty(),
                    quad_operand_empty(), quad_operand_empty());

            /* generate if quads */
            quad_backpatch(jump_to_if);
            res.num = ++global_count;
            res.typ = type_int(); /* TODO: fix this??? */
            res.offset = 0;
            reserved = quad_operand_simple(quad_temporary(res));

            if_operand = AST_expr_quad_generate(e->u.e_if.ethen);
            quad_append_new(quad_opcode_assign, if_operand, quad_operand_empty(),
                    reserved); /* assign to reserved */
            jump_after_else = quad_append_new(quad_opcode_jump, quad_operand_empty(),
                    quad_operand_empty(), quad_operand_empty()); /* generate jump */
            quad_backpatch(jump_after_if);

            /* generate else quads */
            else_operand = AST_expr_quad_generate(e->u.e_if.eelse);

            quad_append_new(quad_opcode_assign, else_operand, quad_operand_empty(),
                    reserved); /* assign to reserved */
            quad_backpatch(jump_after_else);

            return reserved;

        case EXPR_while:
            /* expr1_type = AST_expr_quad_generate(e->u.e_while.econd); */
            /* if ( !type_eq(expr1_type, type_bool()) ) */
            /*     error("Type mismatch: Condition is not of type bool\n"); */
            /* return AST_expr_quad_generate(e->u.e_while.ebody); */
            return quad_operand_empty(); /* TODO */

        case EXPR_for:  
            /* scope_open(symbol_table); */
            /* entry = symbol_enter(symbol_table, e->u.e_for.id, 0); */
            /* entry->entry_type = ENTRY_IDENTIFIER; */
            /* entry->e.identifier.type = type_int(); */
            /* expr1_type = AST_expr_quad_generate(e->u.e_for.expr1); */
            /* if ( !type_eq(expr1_type, type_int()) ) */
            /*     error("Type mismatch: Start part of 'for' is not of type int\n");              */
            /* expr2_type = AST_expr_quad_generate(e->u.e_for.expr2);  */
            /* if ( !type_eq(expr2_type, type_int()) ) */
            /*     error("Type mismatch: End part of 'for' is not of type int\n"); */
            /* expr3_type = AST_expr_quad_generate(e->u.e_for.ebody);  */
            /* if ( !type_eq(expr3_type, type_unit()) ) */
            /*     error("Type mismatch: Expression of 'for' is not of type unit\n"); */
            /* scope_close(symbol_table); */
            /* return type_unit(); */
            return quad_operand_empty(); /* TODO */

        case EXPR_match:
            /* expr1_type = AST_expr_quad_generate(e->u.e_match.expr); */
            /* AST_clause_list_quad_generate(e->u.e_match.list, expr1_type); */
            /* return NULL; */
            return quad_operand_empty(); /* TODO */

        default:
            internal("invalid AST");
    }

    return quad_operand_empty(); /* TODO */
}

void AST_clause_quad_generate(AST_clause c, Type type) {
    if (c == NULL) {
        return;
    }
    AST_pattern_quad_generate(c->pattern, type);
    /*       ... and this AST_expr_print(c->expr); */
}

void AST_pattern_quad_generate(AST_pattern p, Type type) {
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

        case PATTERN_Id: /* TODO */
            /* entry = symbol_lookup(type_symbol_table, p->u.p_Id.id, LOOKUP_ALL_SCOPES, 1); */
            /* if ( !type_eq(type, entry->e.constructor.type) ) */
            /*     error("Type mismatch: Constructor in pattern does not match the type of the expression\n"); */

            /* type = entry->e.constructor.argument_type; */
            /* AST_pattern_list_quad_generate(p->u.p_Id.list, type); */

            break;

        default:
            internal("invalid AST");
    }
}

Quad_operand AST_unop_quad_generate(AST_unop op, Quad_operand operand) {
    
    Quad newquad;
    Quad_operand result;
    Temporary res;   
    
    switch (op) {
        case ast_unop_plus:
            return operand;
        case ast_unop_minus:
            res.num = ++global_count;
            res.typ = type_int();
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_times, quad_operand_simple(quad_iconst(-1)), operand, result);       
            return result;

        case ast_unop_fplus:
            return operand;
        case ast_unop_fminus:
            res.num = ++global_count;
            res.typ = type_float();
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_times, quad_operand_simple(quad_fconst(-1)), operand, result);       
            return result;

        case ast_unop_exclam: /*TODO Ti typo tha valoume sto deref? */
            res.num = ++global_count;
            res.typ = type_float();
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_assign, quad_operand_deref(operand->u.simple), quad_operand_empty(), result);       
            return result;     

        case ast_unop_not: 
            res.num = ++global_count;
            res.typ = type_bool();
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_not, operand, quad_operand_empty(), result);       
            return result;

        default:
            internal("invalid AST");
    }

    return NULL;
}


Quad_operand AST_binop_quad_generate(Quad_operand operand1, AST_expr e, Quad_operand operand2) {
    Quad newquad;
    Quad_operand result;
    Temporary res;

    switch (e->u.e_binop.op) {
        case ast_binop_plus:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_plus, operand1, operand2, result);

            return result;

        case ast_binop_minus:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_minus, operand1, operand2, result);
            return result;
        
        case ast_binop_times:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_times, operand1, operand2, result);
            return result;

        case ast_binop_div:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_div, operand1, operand2, result);
            return result;


        case ast_binop_mod:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_mod, operand1, operand2, result);
            return result;

        case ast_binop_fplus:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_plus, operand1, operand2, result);
            return result;

        case ast_binop_fminus:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_minus, operand1, operand2, result);
            return result;

        case ast_binop_ftimes:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_times, operand1, operand2, result);
            return result;

        case ast_binop_fdiv:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_div, operand1, operand2, result);
            return result;    

        case ast_binop_exp:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_exp, operand1, operand2, result);
            return result;

        case ast_binop_lt:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_lt, operand1, operand2, result);
            return result; 

        case ast_binop_gt:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_gt, operand1, operand2, result);
            return result;

        case ast_binop_le:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_le, operand1, operand2, result);
            return result; 

        case ast_binop_ge:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_ge, operand1, operand2, result);
            return result; 

        case ast_binop_eq:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_eq, operand1, operand2, result);
            return result;

        case ast_binop_ne:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_ne, operand1, operand2, result);
            return result; 

        case ast_binop_pheq:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_eqeq, operand1, operand2, result);
            return result; 

        case ast_binop_phne:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_nxeq, operand1, operand2, result);
            return result; 

        case ast_binop_and:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_and, operand1, operand2, result);
            return result; 

        case ast_binop_or:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_or, operand1, operand2, result);
            return result; 

        case ast_binop_semicolon:     /* TODO */
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_div, operand1, operand2, result);
            return result; 

        case ast_binop_assign:
            res.num = ++global_count;
            res.typ = e->type;
            res.offset = 0;

            result = quad_operand_simple(quad_temporary(res));
            quad_append_new(quad_opcode_assign, operand1, operand2, result);
            return result;

        return quad_operand_empty(); /* TODO */

        default:
            internal("invalid AST");
    }


    return NULL;
}

void AST_ltdef_list_quad_generate(AST_ltdef_list l)
{
    if (l == NULL) return;

    switch (l->kind) {
        case LTDEF_let:
            AST_letdef_quad_generate(l->head.letdef);
            AST_ltdef_list_quad_generate(l->tail);
            break;
        case LTDEF_type:
            AST_typedef_quad_generate(l->head.typdef);
            AST_ltdef_list_quad_generate(l->tail);
            break;
        default:
            internal("invalid AST");
    }
}

void AST_def_list_quad_generate(AST_def_list l)
{
    if (l == NULL) return;

    AST_def_quad_generate(l->head);
    AST_def_list_quad_generate(l->tail);
}

void AST_tdef_list_quad_generate(AST_tdef_list l, Scope scope)
{
    if (l == NULL) return;

    AST_tdef_quad_generate(l->head, scope);
    AST_tdef_list_quad_generate(l->tail, scope);
}

void AST_constr_list_quad_generate(AST_constr_list l, Type user_type)
{
    if (l == NULL) return;

    AST_constr_quad_generate(l->head, user_type);
    AST_constr_list_quad_generate(l->tail, user_type);
}

Type AST_par_list_quad_generate(AST_par_list l)
{
    Type temp, temp2;

    if (l == NULL) return NULL;

    temp = AST_par_quad_generate(l->head);
    temp2 = AST_par_list_quad_generate(l->tail);

    if ( l->tail != NULL )
        return type_func(temp, temp2);
    else
        return temp;
}

/* int AST_expr_list_count(AST_expr_list l) { */
/*     if (l == NULL) return 0; */
/*  */
/*     return 1 + AST_expr_list_count(l->tail); */
/* } */

Type AST_expr_list_quad_generate(AST_expr_list l) {
    Type temp, temp2;

    if (l == NULL) return NULL;

    temp = AST_expr_quad_generate(l->head);
    temp2 =AST_expr_list_quad_generate(l->tail);

    if ( l->tail != NULL )
        return type_func(temp, temp2);
    else
        return temp;
}

void AST_clause_list_quad_generate(AST_clause_list l, Type type)
{
    if (l == NULL) return;

    AST_clause_quad_generate(l->head, type);
    AST_clause_list_quad_generate(l->tail, type);
}

void AST_pattern_list_quad_generate(AST_pattern_list l, Type type) {
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
        AST_pattern_quad_generate(l->head, type->u.t_func.type1);
        AST_pattern_list_quad_generate(l->tail, type->u.t_func.type2);
    }
    else {
        AST_pattern_quad_generate(l->head, type);
        AST_pattern_list_quad_generate(l->tail, NULL);
    }
}

Type Type_list_quad_generate(Type_list l) {
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
    temp2 = Type_list_quad_generate(l->tail);

    if ( temp2 == NULL ) return temp;
    return type_func(temp, temp2);
}
