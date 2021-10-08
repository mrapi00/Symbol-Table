/*--------------------------------------------------------------------*/
/* symtablehash.c                                                     */
/* Author: Mahmudul Rapi                                              */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include "symtable.h"

/*--------------------------------------------------------------------*/

enum { BUCKET_COUNT = 509 };

struct Binding {
    /* The key. */
    const char *key;
     /* The value. */
    void* value;
    /* The address of the next Binding (with same Hash). */
    struct Binding *psNextNode;
};

struct SymTable {
    struct Binding *buckets[BUCKET_COUNT];
};

/*--------------------------------------------------------------------*/

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
        inclusive. */
        
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
    {
    const size_t HASH_MULTIPLIER = 65599;        
    size_t u;
    size_t uHash = 0;
    
    assert(pcKey != NULL);

    for (u = 0; pcKey[u] != '\0'; u++)
    uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];
        
    return uHash % uBucketCount;
    }

/*--------------------------------------------------------------------*/

/* Increase the physical length of oSymTable.  Return 1 (TRUE) if
   successful, or 0 (FALSE) if insufficient memory is available. */

static int SymTable_grow(SymTable_T oSymTable)
{
   
}

/*--------------------------------------------------------------------*/

SymTable_T SymTable_new(void) {
    const size_t INITIAL_PHYS_LENGTH = 509 ;
}

/*--------------------------------------------------------------------*/

void SymTable_free(SymTable_T oSymTable);

/*--------------------------------------------------------------------*/

size_t SymTable_getLength(SymTable_T oSymTable);

/*--------------------------------------------------------------------*/

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue);

/*--------------------------------------------------------------------*/

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
    const void *pvValue);

/*--------------------------------------------------------------------*/

int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/*--------------------------------------------------------------------*/

void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/*--------------------------------------------------------------------*/
 
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/*--------------------------------------------------------------------*/

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra);
