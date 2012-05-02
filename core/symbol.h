/******************************************************************************
 *  CVS version:
 *     $Id: symbol.h,v 1.2 2005/04/07 11:22:47 nickie Exp $
 ******************************************************************************
 *
 *  C header file : symbol.h
 *  Project       : Llama Compiler
 *  Version       : 1.0 alpha
 *  Description   : Generic symbol table
 *
 *  Comments: (in Greek iso-8859-7)
 *  ---------
 *  Εθνικό Μετσόβιο Πολυτεχνείο.
 *  Σχολή Ηλεκτρολόγων Μηχανικών και Μηχανικών Υπολογιστών.
 *  Τομέας Τεχνολογίας Πληροφορικής και Υπολογιστών.
 *  Εργαστήριο Τεχνολογίας Λογισμικού
 */


#ifndef __SYMBOL_H__
#define __SYMBOL_H__


/* ---------------------------------------------------------------------
   ---------------------------- Header files ---------------------------
   --------------------------------------------------------------------- */

#include <stdbool.h>


/* ---------------------------------------------------------------------
   -------------------------- Τύποι δεδομένων --------------------------
   --------------------------------------------------------------------- */

/* Πίνακας συμβόλων */

typedef struct SymbolTable_tag * SymbolTable;

/* Εγγραφή στον πίνακα συμβόλων */

typedef struct SymbolEntry_tag * SymbolEntry;

/* Σύνολο εγγραφών που βρίσκονται στην ίδια εμβέλεια */

typedef struct Scope_tag * Scope;

/* Τρόπος αναζήτησης στον πίνακα συμβόλων */

typedef enum {              /* Ψάξε:                             */
    LOOKUP_CURRENT_SCOPE,   /* - μόνο στην τρέχουσα εμβέλεια,
                                 ακόμα και αν είναι κρυφή        */
    LOOKUP_ALL_SCOPES       /* - σε όλες τις μη κρυφές εμβέλειες */
} LookupType;


/* ---------------------------------------------------------------------
   ------ Πρωτότυπα των συναρτήσεων χειρισμού του πίνακα συμβολών ------
   --------------------------------------------------------------------- */

SymbolTable symbol_make   (unsigned int size);
Scope       scope_open    (SymbolTable table);
Scope       scope_close   (SymbolTable table);
void        scope_hide    (Scope scope, bool flag);
void        scope_insert  (SymbolTable table, Scope scope);
SymbolEntry symbol_enter  (SymbolTable table, Identifier id, bool err);
SymbolEntry symbol_lookup (SymbolTable table, Identifier id, LookupType type,
                           bool err);


#endif
