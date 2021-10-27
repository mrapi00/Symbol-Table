/*--------------------------------------------------------------------*/
/* symtable.h                                                         */
/* Author: Mahmudul Rapi                                              */
/*--------------------------------------------------------------------*/

#ifndef SYMTABLE_INCLUDED
#define SYMTABLE_INCLUDED
/*--------------------------------------------------------------------*/

#include <stddef.h>

/*--------------------------------------------------------------------*/

/* A symbol table is an unordered collection of bindings. */

typedef struct SymTable *SymTable_T;

/*--------------------------------------------------------------------*/

/* Returns a new SymTable object that contains no bindings, or NULL if
   insufficient memory */

SymTable_T SymTable_new(void);

/*--------------------------------------------------------------------*/

/* Frees all memory occupied by oSymTable. */

void SymTable_free(SymTable_T oSymTable);

/*--------------------------------------------------------------------*/

/* Returns the number of bindings in oSymTable. */

size_t SymTable_getLength(SymTable_T oSymTable);

/*--------------------------------------------------------------------*/

/* Adds a new binding to oSymTable consisting of key pcKey and value 
   pvValue if oSymTable does not contain a binding with key pcKey, and 
   returns 1 (TRUE). Otherwise the function leaves oSymTable unchanged 
   and return 0 (FALSE). If insufficient memory is available, then the 
   function leaves oSymTable unchanged and returns 0 (FALSE). */

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue);

/*--------------------------------------------------------------------*/

/* Replace the binding's value with pvValue if oSymTable contains a 
   binding with key pcKey, and returns the old value. Otherwise, 
   leaves oSymTable unchanged and returns NULL.*/

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
    const void *pvValue);

/*--------------------------------------------------------------------*/

/* Returns 1 (TRUE) if oSymTable contains a binding whose key is pcKey, 
   and 0 (FALSE) otherwise. */ 

int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/*--------------------------------------------------------------------*/

/* Returns the value of the binding within oSymTable whose key is pcKey, 
   or NULL if no such binding exists. */

void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/*--------------------------------------------------------------------*/

/* If oSymTable contains a binding with key pcKey, removes the binding 
   from oSymTable and returns the binding's value. Otherwise, leaves 
   oSymTable unchanged and returns NULL. */
   
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/*--------------------------------------------------------------------*/
/* Applies function *pfApply to each binding in oSymTable, passing 
   pvExtra as an extra parameter. The function calls 
   (*pfApply)(pcKey, pvValue, pvExtra) for each pcKey/pvValue binding 
   in oSymTable. */

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra);

/*--------------------------------------------------------------------*/
#endif