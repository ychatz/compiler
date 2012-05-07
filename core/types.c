/******************************************************************************
 *  CVS version:
 *     $Id: types.c,v 1.2 2005/04/07 11:22:47 nickie Exp $
 ******************************************************************************
 *
 *  C code file : types.c
 *  Project     : Llama Compiler
 *  Version     : 1.0 alpha
 *  Description : Types
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

#include "types.h"


/* ---------------------------------------------------------------------
   --------------------- Υλοποίηση συναρτήσεων -------------------------
   --------------------------------------------------------------------- */

Type type_unit ()
{
   Type result = new(sizeof(struct Type_tag));

   result->kind = TYPE_unit;
   return result;
}

Type type_bool ()
{
   Type result = new(sizeof(struct Type_tag));

   result->kind = TYPE_bool;
   return result;
}

Type type_char ()
{
   Type result = new(sizeof(struct Type_tag));

   result->kind = TYPE_char;
   return result;
}

Type type_int ()
{
   Type result = new(sizeof(struct Type_tag));

   result->kind = TYPE_int;
   return result;
}

Type type_float ()
{
   Type result = new(sizeof(struct Type_tag));

   result->kind = TYPE_float;
   return result;
}

Type type_id (Identifier id)
{
   Type result = new(sizeof(struct Type_tag));

   result->kind = TYPE_id;
   result->u.t_id.id = id;
   return result;
}

Type type_array (int dim, Type type)
{
   Type result = new(sizeof(struct Type_tag));

   result->kind = TYPE_array;
   result->u.t_array.dim = dim;
   result->u.t_array.type = type;
   return result;
}

Type type_ref (Type type)
{
   Type result = new(sizeof(struct Type_tag));

   result->kind = TYPE_ref;
   result->u.t_ref.type = type;
   return result;
}

Type type_func (Type type1, Type type2)
{
   Type result = new(sizeof(struct Type_tag));

   result->kind = TYPE_func;
   result->u.t_func.type1 = type1;
   result->u.t_func.type2 = type2;
   return result;
}


Type type_unknown() {
   Type result = new(sizeof(struct Type_tag));
   result->kind = TYPE_unknown;
   return result;
}
