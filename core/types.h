/******************************************************************************
 *  CVS version:
 *     $Id: types.h,v 1.1 2005/04/07 10:31:32 nickie Exp $
 ******************************************************************************
 *
 *  C header file : types.h
 *  Project       : Llama Compiler
 *  Version       : 1.0 alpha
 *  Description   : Types
 *
 *  Comments: (in Greek iso-8859-7)
 *  ---------
 *  Εθνικό Μετσόβιο Πολυτεχνείο.
 *  Σχολή Ηλεκτρολόγων Μηχανικών και Μηχανικών Υπολογιστών.
 *  Τομέας Τεχνολογίας Πληροφορικής και Υπολογιστών.
 *  Εργαστήριο Τεχνολογίας Λογισμικού
 */


#ifndef __TYPES_H__
#define __TYPES_H__


/* ---------------------------------------------------------------------
   ---------------------------- Header files ---------------------------
   --------------------------------------------------------------------- */

#include <stdbool.h>

#include "general.h"


/* ---------------------------------------------------------------------
   ------------------------- Ορισμός τύπων -----------------------------
   --------------------------------------------------------------------- */

/* Δήλωση */

typedef struct Type_tag * Type;


/* Ορισμός δομής */

struct Type_tag {
    enum {                               /***** Το είδος του τύπου ****/
        TYPE_unit,                        /* unit                      */
        TYPE_bool,                        /* bool                      */
        TYPE_char,                        /* char                      */
        TYPE_int,                         /* int                       */
        TYPE_float,                       /* float                     */
        TYPE_array,                       /* array type                */
        TYPE_ref,                         /* reference type (ref)      */
        TYPE_func,                        /* function (arrow)          */
        TYPE_id,                           /* user defined type (id)    */
        TYPE_unknown
    } kind;
    union {
        struct {
            int dim;
            Type type;
        } t_array;
        struct {
            Type type;
        } t_ref;
        struct {
            Type type1;
            Type type2;
        } t_func;
        struct {
            Identifier id;
        } t_id;
    } u;
};



/* ---------------------------------------------------------------------
   --------------------- Πρωτότυπα συναρτήσεων -------------------------
   --------------------------------------------------------------------- */

Type type_unit  ();
Type type_int   ();
Type type_char  ();
Type type_bool  ();
Type type_float ();
Type type_func  (Type t1, Type t2);
Type type_ref   (Type t);
Type type_array (int dim, Type t);
Type type_id    (Identifier id);
Type type_unknown ();


#endif
