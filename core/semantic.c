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
#include "error.h"
#include "ast.h"
#include "semantic.h"
#include "symbol.h"

SymbolTable symbol_table;

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
        default:
            internal("Can't convert expression tag to hash\n");
    }

    return id_make(temp);
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
    symbol_table = symbol_make(193); /* TODO: find a better size */
    scope_open(symbol_table);
    AST_ltdef_list_traverse(p->list);
    scope_close(symbol_table);
}

void AST_letdef_traverse (AST_letdef ld)
{
    if (ld == NULL) {
        /* fprintf(f, "<<NULL>>\n"); */
        return;
    }
    if ( ld->recFlag ) {
        /* ... */
    }
    else {
        /* ... */
    }
    AST_def_list_traverse(ld->list);
}
/*  */
/* void AST_typedef_print (FILE * f, int prec, AST_typedef td) */
/* { */
/*    indent(f, prec); */
/*    if (td == NULL) { */
/*       fprintf(f, "<<NULL>>\n"); */
/*       return; */
/*    } */
/*    fprintf(f, "ast_typedef (\n"); */
/*    AST_tdef_list_print(f, prec+1, td->list); */
/*    indent(f, prec); fprintf(f, ")\n"); */
/* } */

void AST_def_traverse (AST_def d)
{
    SymbolEntry entry;
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
            /* case DEF_mutable: */
            /*    fprintf(f, "ast_def: mutable (\n"); */
            /*    Identifier_print(f, prec+1, d->u.d_mutable.id); */
            /*    AST_expr_list_print(f, prec+1, d->u.d_mutable.list); */
            /*    Type_print(f, prec+1, d->u.d_mutable.type); */
            /*    indent(f, prec); fprintf(f, ")\n"); */
            /*    break; */
        default:
            internal("invalid AST");
    }
}

/* void AST_tdef_print (FILE * f, int prec, AST_tdef td) */
/* { */
/*    indent(f, prec); */
/*    if (td == NULL) { */
/*       fprintf(f, "<<NULL>>\n"); */
/*       return; */
/*    } */
/*    fprintf(f, "ast_tdef (\n"); */
/*    Identifier_print(f, prec+1, td->id); */
/*    AST_constr_list_print(f, prec+1, td->list); */
/*    indent(f, prec); fprintf(f, ")\n"); */
/* } */
/*  */
/* void AST_constr_print (FILE * f, int prec, AST_constr c) */
/* { */
/*    indent(f, prec); */
/*    if (c == NULL) { */
/*       fprintf(f, "<<NULL>>\n"); */
/*       return; */
/*    } */
/*    fprintf(f, "ast_constr (\n"); */
/*    Identifier_print(f, prec+1, c->id); */
/*    Type_list_print(f, prec+1, c->list); */
/*    indent(f, prec); fprintf(f, ")\n"); */
/* } */

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
    Type expr1_type, expr2_type;

    if (e == NULL) {
        /* fprintf(f, "<<NULL>>\n"); */
        return NULL;
    }
    switch (e->kind) {
        case EXPR_iconst:
            entry = symbol_enter(symbol_table, to_hash(e), 0);
            entry->entry_type = ENTRY_CONSTANT;
            entry->e.constant.type = type_int();
            entry->e.constant.value.v_int = e->u.e_iconst.rep;

            return entry->e.constant.type;
            break;
        case EXPR_fconst:
            entry = symbol_enter(symbol_table, to_hash(e), 0);
            entry->entry_type = ENTRY_CONSTANT;
            entry->e.constant.type = type_float();
            entry->e.constant.value.v_float = e->u.e_fconst.rep;

            return entry->e.constant.type;
            break;
        case EXPR_cconst:
            entry = symbol_enter(symbol_table, to_hash(e), 0);
            entry->entry_type = ENTRY_CONSTANT;
            entry->e.constant.type = type_char();
            entry->e.constant.value.v_char = e->u.e_cconst.rep;

            return entry->e.constant.type;
            break;
        case EXPR_strlit:
            entry = symbol_enter(symbol_table, to_hash(e), 0);
            entry->entry_type = ENTRY_CONSTANT;
            entry->e.constant.type = type_array(1, type_char());
            entry->e.constant.value.v_strlit = e->u.e_strlit.rep;

            return entry->e.constant.type;
            break;
        /* case EXPR_true: */
        /*     fprintf(f, "ast_expr: true\n"); */
        /*     break; */
        /* case EXPR_false: */
        /*     fprintf(f, "ast_expr: false\n"); */
        /*     break; */
        /* case EXPR_unit: */
        /*     fprintf(f, "ast_expr: unit\n"); */
        /*     break; */
        /* case EXPR_unop: */
        /*     fprintf(f, "ast_expr: unop (\n"); */
        /*     AST_unop_print(f, prec+1, e->u.e_unop.op); */
        /*     AST_expr_print(f, prec+1, e->u.e_unop.expr); */
        /*     indent(f, prec); fprintf(f, ")\n"); */
        /*     break; */
        case EXPR_binop:
            expr1_type = AST_expr_traverse(e->u.e_binop.expr1);
            expr2_type = AST_expr_traverse(e->u.e_binop.expr2);
            AST_binop_traverse(expr1_type, e->u.e_binop.op, expr2_type);
            break;
        /* case EXPR_id: */
        /*     fprintf(f, "ast_expr: id (\n"); */
        /*     Identifier_print(f, prec+1, e->u.e_id.id); */
        /*     indent(f, prec); fprintf(f, ")\n"); */
        /*     break; */
        /* case EXPR_Id: */
        /*     fprintf(f, "ast_expr: Id (\n"); */
        /*     Identifier_print(f, prec+1, e->u.e_Id.id); */
        /*     indent(f, prec); fprintf(f, ")\n"); */
        /*     break; */
        /* case EXPR_call: */
        /*     fprintf(f, "ast_expr: call (\n"); */
        /*     Identifier_print(f, prec+1, e->u.e_call.id); */
        /*     AST_expr_list_print(f, prec+1, e->u.e_call.list); */
        /*     indent(f, prec); fprintf(f, ")\n"); */
        /*     break; */
        /* case EXPR_Call: */
        /*     fprintf(f, "ast_expr: Call (\n"); */
        /*     Identifier_print(f, prec+1, e->u.e_Call.id); */
        /*     AST_expr_list_print(f, prec+1, e->u.e_Call.list); */
        /*     indent(f, prec); fprintf(f, ")\n"); */
        /*     break; */
        /* case EXPR_arrel: */
        /*     fprintf(f, "ast_expr: arrel (\n"); */
        /*     Identifier_print(f, prec+1, e->u.e_arrel.id); */
        /*     AST_expr_list_print(f, prec+1, e->u.e_arrel.list); */
        /*     indent(f, prec); fprintf(f, ")\n"); */
        /*     break; */
        /* case EXPR_dim: */
        /*     fprintf(f, "ast_expr: dim (\n"); */
        /*     indent(f, prec+1); */
        /*     fprintf(f, "dim = %d\n", e->u.e_dim.dim); */
        /*     Identifier_print(f, prec+1, e->u.e_dim.id); */
        /*     indent(f, prec); fprintf(f, ")\n"); */
        /*     break; */
        /* case EXPR_new: */
        /*     fprintf(f, "ast_expr: new (\n"); */
        /*     Type_print(f, prec+1, e->u.e_new.type); */
        /*     indent(f, prec); fprintf(f, ")\n"); */
        /*     break; */
        /* case EXPR_delete: */
        /*     fprintf(f, "ast_expr: delete (\n"); */
        /*     AST_expr_print(f, prec+1, e->u.e_delete.expr); */
        /*     indent(f, prec); fprintf(f, ")\n"); */
        /*     break; */
        /* case EXPR_let: */
        /*     fprintf(f, "ast_expr: let (\n"); */
        /*     AST_letdef_print(f, prec+1, e->u.e_let.def); */
        /*     AST_expr_print(f, prec+1, e->u.e_let.expr); */
        /*     indent(f, prec); fprintf(f, ")\n"); */
        /*     break; */
        /* case EXPR_if: */
        /*     fprintf(f, "ast_expr: if (\n"); */
        /*     AST_expr_print(f, prec+1, e->u.e_if.econd); */
        /*     AST_expr_print(f, prec+1, e->u.e_if.ethen); */
        /*     AST_expr_print(f, prec+1, e->u.e_if.eelse); */
        /*     indent(f, prec); fprintf(f, ")\n"); */
        /*     break; */
        /* case EXPR_while: */
        /*     fprintf(f, "ast_expr: while (\n"); */
        /*     AST_expr_print(f, prec+1, e->u.e_while.econd); */
        /*     AST_expr_print(f, prec+1, e->u.e_while.ebody); */
        /*     indent(f, prec); fprintf(f, ")\n"); */
        /*     break; */
        /* case EXPR_for: */
        /*     fprintf(f, "ast_expr: for (\n"); */
        /*     Identifier_print(f, prec+1, e->u.e_for.id); */
        /*     AST_expr_print(f, prec+1, e->u.e_for.expr1); */
        /*     indent(f, prec+1); */
        /*     fprintf(f, "downFlag = %s\n", e->u.e_for.downFlag ? "true" : "false"); */
        /*     AST_expr_print(f, prec+1, e->u.e_for.expr2); */
        /*     AST_expr_print(f, prec+1, e->u.e_for.ebody); */
        /*     indent(f, prec); fprintf(f, ")\n"); */
        /*     break; */
        /* case EXPR_match: */
        /*     fprintf(f, "ast_expr: match (\n"); */
        /*     AST_expr_print(f, prec+1, e->u.e_match.expr); */
        /*     AST_clause_list_print(f, prec+1, e->u.e_match.list); */
        /*     indent(f, prec); fprintf(f, ")\n"); */
        /*     break; */
        default:
            internal("invalid AST");
    }

    return NULL;
}

/* void AST_clause_print (FILE * f, int prec, AST_clause c) */
/* { */
/*    indent(f, prec); */
/*    if (c == NULL) { */
/*       fprintf(f, "<<NULL>>\n"); */
/*       return; */
/*    } */
/*    fprintf(f, "ast_clause (\n"); */
/*    AST_pattern_print(f, prec+1, c->pattern); */
/*    AST_expr_print(f, prec+1, c->expr); */
/*    indent(f, prec); fprintf(f, ")\n"); */
/* } */
/*  */
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
/*  */
/* void AST_unop_print (FILE * f, int prec, AST_unop op) */
/* { */
/*    indent(f, prec); */
/*    switch (op) { */
/*       case ast_unop_plus: */
/*          fprintf(f, "ast_unop_plus\n"); */
/*          break; */
/*       case ast_unop_minus: */
/*          fprintf(f, "ast_unop_minus\n"); */
/*          break; */
/*       case ast_unop_fplus: */
/*          fprintf(f, "ast_unop_fplus\n"); */
/*          break; */
/*       case ast_unop_fminus: */
/*          fprintf(f, "ast_unop_fminus\n"); */
/*          break; */
/*       case ast_unop_exclam: */
/*          fprintf(f, "ast_unop_exclam\n"); */
/*          break; */
/*       case ast_unop_not: */
/*          fprintf(f, "ast_unop_not\n"); */
/*          break; */
/*       default: */
/*          internal("invalid AST"); */
/*    } */
/* } */

Type AST_binop_traverse (Type expr1, AST_binop op, Type expr2) {
    switch (op) {
        case ast_binop_plus:
        case ast_binop_minus:
        case ast_binop_times:
        case ast_binop_div:
            if ( expr1->kind != TYPE_int ) error("Type mismatch in the left argument\n");
            if ( expr2->kind != TYPE_int ) error("Type mismatch in the right argument\n");
            return type_int();

        case ast_binop_fplus:
        case ast_binop_fminus:
        case ast_binop_ftimes:
        case ast_binop_fdiv:
            if ( expr1->kind != TYPE_float ) error("Type mismatch in the left argument\n");
            if ( expr2->kind != TYPE_float ) error("Type mismatch in the right argument\n");
            return type_float();

        /* case ast_binop_mod: */
        /*     fprintf(f, "ast_binop_mod\n"); */
        /*     break; */
        /* case ast_binop_exp: */
        /*     fprintf(f, "ast_binop_exp\n"); */
        /*     break; */

        case ast_binop_eq:
        case ast_binop_ne:
        case ast_binop_lt:
        case ast_binop_gt:
            if ( expr1->kind != expr2->kind )
                error("Type mismatch: Arguments must be of the same type\n");
            if ( expr1->kind != TYPE_char && expr1->kind != TYPE_float && expr1->kind != TYPE_int )
                error("Type mismatch: Left argument must be of type char, float or int\n");
            if ( expr2->kind != TYPE_char && expr2->kind != TYPE_float && expr2->kind != TYPE_int )
                error("Type mismatch: Right argument must be of type char, float or int\n");
            return expr1;

        /* case ast_binop_le: */
        /*     fprintf(f, "ast_binop_le\n"); */
        /*     break; */
        /* case ast_binop_ge: */
        /*     fprintf(f, "ast_binop_ge\n"); */
        /*     break; */
        /* case ast_binop_pheq: */
        /*     fprintf(f, "ast_binop_pheq\n"); */
        /*     break; */
        /* case ast_binop_phne: */
        /*     fprintf(f, "ast_binop_phne\n"); */
        /*     break; */

        case ast_binop_and:
        case ast_binop_or:
            if ( expr1->kind != TYPE_bool ) error("Type mismatch in the left argument\n");
            if ( expr2->kind != TYPE_bool ) error("Type mismatch in the right argument\n");
            return type_bool();

        /* case ast_binop_semicolon: */
        /*     fprintf(f, "ast_binop_semicolon\n"); */
        /*     break; */
        /* case ast_binop_assign: */
        /*     fprintf(f, "ast_binop_assign\n"); */
        /*     break; */
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
    switch (l->kind) {
        case LTDEF_let:
            scope_open(symbol_table);
            AST_letdef_traverse(l->head.letdef);
            AST_ltdef_list_traverse(l->tail);
            scope_close(symbol_table);
            break;
            /* case LTDEF_type: */
            /*     scope_open(symbol_table); */
            /*     AST_typedef_traverse(l->head.typdef); */
            /*     AST_ltdef_list_traverse(l->tail); */
            /*     scope_close(symbol_table); */
            /*     break; */
        default:
            internal("invalid AST");
    }
}

void AST_def_list_traverse (AST_def_list l)
{
    if (l == NULL) {
        /* fprintf(f, "<<NULL>>\n"); */
        return;
    }
    AST_def_traverse(l->head);
    AST_def_list_traverse(l->tail);
}

/* void AST_tdef_list_print (FILE * f, int prec, AST_tdef_list l) */
/* { */
/*    indent(f, prec); */
/*    if (l == NULL) { */
/*       fprintf(f, "<<NULL>>\n"); */
/*       return; */
/*    } */
/*    fprintf(f, "ast_tdef_list (\n"); */
/*    AST_tdef_print(f, prec+1, l->head); */
/*    AST_tdef_list_print(f, prec+1, l->tail); */
/*    indent(f, prec); fprintf(f, ")\n"); */
/* } */
/*  */
/* void AST_constr_list_print (FILE * f, int prec, AST_constr_list l) */
/* { */
/*    indent(f, prec); */
/*    if (l == NULL) { */
/*       fprintf(f, "<<NULL>>\n"); */
/*       return; */
/*    } */
/*    fprintf(f, "ast_constr_list (\n"); */
/*    AST_constr_print(f, prec+1, l->head); */
/*    AST_constr_list_print(f, prec+1, l->tail); */
/*    indent(f, prec); fprintf(f, ")\n"); */
/* } */

void AST_par_list_traverse (AST_par_list l)
{
    if (l == NULL) {
        /*fprintf(f, "<<NULL>>\n");*/
        return;
    }
    AST_par_traverse(l->head);
    AST_par_list_traverse(l->tail);
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
/*  */
/* void AST_clause_list_print (FILE * f, int prec, AST_clause_list l) */
/* { */
/*    indent(f, prec); */
/*    if (l == NULL) { */
/*       fprintf(f, "<<NULL>>\n"); */
/*       return; */
/*    } */
/*    fprintf(f, "ast_clause_list (\n"); */
/*    AST_clause_print(f, prec+1, l->head); */
/*    AST_clause_list_print(f, prec+1, l->tail); */
/*    indent(f, prec); fprintf(f, ")\n"); */
/* } */
/*  */
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
/* void Type_list_print (FILE * f, int prec, Type_list l) */
/* { */
/*    indent(f, prec); */
/*    if (l == NULL) { */
/*       fprintf(f, "<<NULL>>\n"); */
/*       return; */
/*    } */
/*    fprintf(f, "ast_type_list (\n"); */
/*    Type_print(f, prec+1, l->head); */
/*    Type_list_print(f, prec+1, l->tail); */
/*    indent(f, prec); fprintf(f, ")\n"); */
/* } */
/*  */