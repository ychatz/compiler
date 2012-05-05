/******************************************************************************
 *  CVS version:
 *     $Id: pretty.h,v 1.2 2005/04/07 11:22:47 nickie Exp $
 ******************************************************************************
 *
 *  C header file : pretty.h
 *  Project       : Llama Compiler
 *  Version       : 1.0 alpha
 *  Description   : Pretty printer for abstract syntax
 *
 *  Comments: (in Greek iso-8859-7)
 *  ---------
 *  Εθνικό Μετσόβιο Πολυτεχνείο.
 *  Σχολή Ηλεκτρολόγων Μηχανικών και Μηχανικών Υπολογιστών.
 *  Τομέας Τεχνολογίας Πληροφορικής και Υπολογιστών.
 *  Εργαστήριο Τεχνολογίας Λογισμικού
 */


#ifndef __SEMANTIC_H__
#define __SEMANTIC_H__


/* ---------------------------------------------------------------------
   ---------------------------- Header files ---------------------------
   --------------------------------------------------------------------- */

#include <stdio.h>

#include "ast.h"


/* ---------------------------------------------------------------------
   ------------------------ Function prototypes ------------------------
   --------------------------------------------------------------------- */

/* void Identifier_traverse       (FILE *, int, Identifier); */
/* void Type_traverse             (FILE *, int, Type); */
/*  */
/* void RepInt_traverse           (FILE *, int, RepInt); */
/* void RepFloat_traverse         (FILE *, int, RepFloat); */
/* void RepChar_traverse          (FILE *, int, RepChar); */
/* void RepString_traverse        (FILE *, int, RepString); */

void AST_program_traverse      (AST_program);
void AST_letdef_traverse       (AST_letdef);
/* void AST_typedef_print         (AST_typedef); */
void AST_def_traverse          (AST_def);
/* void AST_tdef_traverse            (AST_tdef); */
/* void AST_constr_traverse          (AST_constr); */
/* void AST_par_traverse             (AST_par); */
Type AST_expr_traverse            (AST_expr);
/* void AST_clause_traverse          (AST_clause); */
/* void AST_pattern_traverse         (AST_pattern); */
/*  */
/* void AST_unop_traverse            (AST_unop); */
void AST_binop_traverse           (Type, AST_binop, Type);

void AST_ltdef_list_traverse   (AST_ltdef_list);
void AST_def_list_traverse     (AST_def_list);
/* void AST_tdef_list_traverse       (AST_tdef_list); */
/* void AST_constr_list_traverse     (AST_constr_list); */
void AST_par_list_traverse        (AST_par_list);
/* void AST_expr_list_traverse       (AST_expr_list); */
/* void AST_clause_list_traverse     (AST_clause_list); */
/* void AST_pattern_list_traverse    (AST_pattern_list); */
/*  */
/* void Type_list_traverse        (Type_list); */


#endif
