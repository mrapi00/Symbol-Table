/*--------------------------------------------------------------------*/
/* symtablehash.c                                                     */
/* Author: Mahmudul Rapi                                              */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "symtable.h"

/*--------------------------------------------------------------------*/

struct Binding {
    /* The key. */
    const char *key;
     /* The value. */
    void *value;
    /* The address of the next Binding (with same Hash). */
    struct Binding *pNextBinding;
};

struct SymTable {
    /* Number of bindings */
    size_t size;
    /* Number of buckets*/
    size_t bucketCount;
    /* Array of bindings */
    struct Binding **buckets;
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

/* Helper function that finds the next bucket count in the sequence 
   of prime number bucket counts, and returns the value.  */

static int growHelper(const int bucketCount){
    assert(bucketCount == 509 || bucketCount == 1021 || 
           bucketCount == 2039 || bucketCount == 4093 || 
           bucketCount == 8191 || bucketCount == 16381 || 
           bucketCount == 32749);

    switch (bucketCount)
    {
    case 509:
        return 1021;
    case 1021:
        return 2039;
    case 2039:
        return 4093;
    case 4093:
        return 8191;
    case 8191:
        return 16381;
    case 16381:
        return 32749;
    case 32749:
        return 65521;
    }

    return -1;
}

/*--------------------------------------------------------------------*/

/* Helper function that allocates memory to an SymTable object by 
   taking in a bucket count as argument. Returns reference to the
   SymTable object. */

static SymTable_T newHelper(int bucketC){
    SymTable_T oSymTable;

    oSymTable = (SymTable_T) malloc(sizeof(struct SymTable));
    if (oSymTable == NULL)
        return NULL;

    oSymTable->buckets = 
        (struct Binding**) calloc(bucketC, sizeof(struct Binding *));

    if (oSymTable->buckets == NULL){
        free(oSymTable);
        return NULL;
    }
    
    oSymTable->size = 0;
    oSymTable->bucketCount = bucketC;
    return oSymTable;
}

/*--------------------------------------------------------------------*/

/* Increase the bucket count of oSymTable.  Return 1 (TRUE) if
   successful, or 0 (FALSE) if insufficient memory is available. */

static int SymTable_grow(SymTable_T oSymTable)
{
    size_t index;
    size_t uNewBucketCount;
    size_t oldBucketCount = oSymTable->bucketCount;
    SymTable_T newSymTable;
    SymTable_T pOld;

    assert(oSymTable != NULL);
    /* Creates empty newSymTable with larger bucket count */
    uNewBucketCount = growHelper(oldBucketCount);
    newSymTable = newHelper(uNewBucketCount);
    if (newSymTable == NULL)
        return 0;
    
    /* Traverses bindings of oSymTable and copies the key-value pairs
       into newSymTable (re-hashed) */

    for (index = 0; index < oldBucketCount; index++){
        struct Binding* currentBind = *(oSymTable->buckets) + index;
        while (currentBind != NULL){
            SymTable_put(newSymTable, currentBind->key, currentBind->value);
            currentBind = currentBind->pNextBinding;
        }    
    }

    /* Make oSymTable reference newSymTable, and free oSymTable's memory */
    pOld = oSymTable;
    oSymTable = newSymTable;
    SymTable_free(pOld);
    return 1;
}

/*--------------------------------------------------------------------*/

SymTable_T SymTable_new(void) {
    enum {INITIAL_BUCKET_COUNT = 509};
    SymTable_T oSymTable = newHelper(INITIAL_BUCKET_COUNT);

    if (oSymTable == NULL)
      return NULL;
    return oSymTable;
}

/*--------------------------------------------------------------------*/

void SymTable_free(SymTable_T oSymTable){
    size_t index;
    size_t bucketC = oSymTable->bucketCount;
    assert(oSymTable != NULL);
    /* Traverses bindings of oSymTable and frees the memory occupied 
       by every binding object */
    for (index = 0; index < bucketC; index++){
        struct Binding* currentBind = *(oSymTable->buckets) + index;
        while (currentBind != NULL){
            struct Binding* pCurrent = currentBind;
            free((char*)pCurrent->key);
            currentBind = currentBind->pNextBinding;
            free(pCurrent);
        }    
    }
    free(oSymTable);
}

/*--------------------------------------------------------------------*/

size_t SymTable_getLength(SymTable_T oSymTable){
    assert(oSymTable != NULL);
    return oSymTable->size;
}

/*--------------------------------------------------------------------*/

int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
                 const void *pvValue){

    enum {MAX_BUCKET_COUNT = 65521};
    int iSuccessful;
    struct Binding* newBinding;
    struct Binding* currBinding;
    size_t index;
    assert(oSymTable != NULL);

    if (SymTable_contains(oSymTable, pcKey)) 
        return 0;

    if (oSymTable->size == oSymTable->bucketCount && 
        oSymTable->bucketCount != MAX_BUCKET_COUNT)
    {
       iSuccessful = SymTable_grow(oSymTable);
       if (!iSuccessful)
          return 0;
    }

    index = SymTable_hash(pcKey, oSymTable->bucketCount);
    
    newBinding = (struct Binding*)malloc(sizeof(struct Binding));
    if (newBinding == NULL)
        return 0;

    newBinding->key = (const char*)malloc(strlen(pcKey) + 1);
    strcpy((char*)newBinding->key, pcKey);
    newBinding->value = (void*) pvValue;
    
    if ((oSymTable->buckets)[index] == NULL){
        (oSymTable->buckets)[index] = newBinding; /* not sure */
    }
    else {
        currBinding = oSymTable->buckets[index];
        while (currBinding->pNextBinding != NULL){
            currBinding = currBinding->pNextBinding;
        }
        *(currBinding->pNextBinding) = *newBinding; /* not sure */
    }

    (oSymTable->size)++;
    return 1;

}

/*--------------------------------------------------------------------*/

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
    const void *pvValue){
    size_t index;
    struct Binding* binding;
    assert(oSymTable != NULL);

    index = SymTable_hash(pcKey, oSymTable->bucketCount);
    binding = (*(oSymTable->buckets) + index);

    while (binding != NULL){
        if (strcmp(pcKey, binding->key) == 0){
            void *oldValue = binding->value;
            binding->value = (void*) pvValue; /* is possible? */
            return oldValue;
        }
        binding = binding->pNextBinding;
    }
    return NULL;
    
    }

/*--------------------------------------------------------------------*/

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
    size_t index;
    struct Binding* binding;
    assert(oSymTable != NULL);

    index = SymTable_hash(pcKey, oSymTable->bucketCount);
    binding = (oSymTable->buckets[index]);

    while (binding != NULL){
        if (strcmp(pcKey, binding->key) == 0)
            return 1;
        binding = binding->pNextBinding;
    }
    return 0;
}

/*--------------------------------------------------------------------*/

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
    size_t index;
    struct Binding* binding;
    assert(oSymTable != NULL);

    index = SymTable_hash(pcKey, oSymTable->bucketCount);
    binding = (*(oSymTable->buckets) + index);

    while (binding != NULL){
        if (strcmp(pcKey, binding->key) == 0)
            return binding->value;
        binding = binding->pNextBinding;
    }
    return NULL;
}

/*--------------------------------------------------------------------*/
 
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
    size_t index;
    bool found = false;
    struct Binding* currBinding;
    struct Binding* prevBinding = NULL;
    void *returnValue;
    assert(oSymTable != NULL);
    
    index = SymTable_hash(pcKey, oSymTable->bucketCount);
    currBinding = (*(oSymTable->buckets) + index);

    while (currBinding != NULL){
        if (strcmp(pcKey, currBinding->key) == 0){
            found = true;
            break;
        }
        prevBinding = currBinding;
        currBinding = currBinding->pNextBinding;
    }

    if (!found)
        return NULL;
    /* missing a case maybe? */
    if (currBinding->pNextBinding != NULL && prevBinding != NULL){
        prevBinding->pNextBinding = currBinding->pNextBinding;
    }
    returnValue = currBinding->value;
    free((char*) currBinding->key);
    free(currBinding);
    return returnValue;
}

/*--------------------------------------------------------------------*/

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra);
