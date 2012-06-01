/******************************************************************************
 *  CVS version:
 *     $Id: general.c,v 1.2 2005/04/07 11:22:47 nickie Exp $
 ******************************************************************************
 *
 *  C code file : general.c
 *  Project     : Llama Compiler
 *  Version     : 1.0 alpha
 *  Description : Generic definitions
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

#include <stdlib.h>
#include <string.h>
#undef USEGC

#if USEGC
#include "gc.h"
#endif

#include "general.h"
#include "error.h"


/* ---------------------------------------------------------------------
   --------------------------- Generic flags ---------------------------
   --------------------------------------------------------------------- */

bool flagVerbose = true;


/* ---------------------------------------------------------------------
   ------------- Ελεγχόμενος τερματισμός του μεταγλωττιστή -------------
   --------------------------------------------------------------------- */

void terminate (int code)
{
    if (flagVerbose)
        message("\r\nErrors:   %d\nWarnings: %d", numErrors, numWarnings);
    exit(code);
}


/* ---------------------------------------------------------------------
   ----------- Υλοποίηση των συναρτήσεων διαχείρισης μνήμης ------------
   --------------------------------------------------------------------- */

void * new (size_t size)
{
    void * result =
#if USEGC
        GC_MALLOC(size);
#else
    malloc(size);
#endif

    if (result == NULL)
        fatal("\rOut of memory");
    return result;
}

void delete (void * p)
{
    if (p != NULL)
#if USEGC
        GC_FREE(p);
#else
    free(p);
#endif
}


/* ---------------------------------------------------------------------
   ------- Αρχείο εισόδου του μεταγλωττιστή και αριθμός γραμμής --------
   --------------------------------------------------------------------- */

char *filename;
int linecount;

/* ---------------------------------------------------------------------
   ------------ Συνάρτηση κατακερματισμού για αναγνωριστικά ------------
   --------------------------------------------------------------------- */

typedef unsigned long int HashType;

static HashType PJW_hash (const char * key)
{
    /*
     *  P.J. Weinberger's hashing function. See also:
     *  Aho A.V., Sethi R. & Ullman J.D, "Compilers: Principles,
     *  Techniques and Tools", Addison Wesley, 1986, pp. 433-437.
     */

    const HashType PJW_OVERFLOW =
        (((HashType) 0xf) << (8 * sizeof(HashType) - 4));
    const int PJW_SHIFT = (8 * (sizeof(HashType) - 1));

    HashType h, g;

    for (h = 0; *key != '\0'; key++) {
        h = (h << 4) + (*key);
        if ((g = h & PJW_OVERFLOW) != 0) {
            h ^= g >> PJW_SHIFT;
            h ^= g;
        }
    }
    return h;
}


/* ---------------------------------------------------------------------
   --------------------------- Αναγνωριστικά ---------------------------
   --------------------------------------------------------------------- */

static Identifier id_make_new (const char * name, Identifier next)
{
    Identifier result = new (sizeof (struct Identifier_tag));

    result->name = new(strlen(name) + 1);
    strcpy(result->name, name);
    result->next = next;
    return result;
}

/* Το μέγεθος του πίνακα κατακερματισμού πρέπει να είναι πρώτος αριθμός */

#define SIZE 193

/* Εκμεταλλευόμαστε το γεγονός ότι οι (static) global μεταβλητές
   αρχικοποιούνται: αρχικά θα έχουμε hashtable[i] == NULL για κάθε i.
   Για την επαναρχικοποίησή του μπορεί να χρησιμοποιηθεί η id_reset. */

static Identifier hashtable [SIZE];

Identifier id_make (const char * name)
{
    HashType index = PJW_hash(name) % SIZE;

    Identifier ids = hashtable[index];
    Identifier id;

    for (id=ids; id != NULL; id=id->next)
        if (strcmp(id->name, name) == 0)
            return id;
    id = id_make_new(name, ids);
    hashtable[index] = id;
    return id;
}

const char * id_name (Identifier id)
{
    return id->name;
}

/* ΠΡΟΣΟΧΗ: Κλήση της παρακάτω συνάρτησης αν ακόμα βρίσκονται αναγνωριστικά σε
   χρήση θα έχει απρόβλεπτες συνέπειες. */

void id_reset ()
{
    int i;

    for (i=0; i<SIZE; i++)
        hashtable[i] = NULL;
}
