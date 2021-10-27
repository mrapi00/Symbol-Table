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
/* Array containing bucket counts for the hash table as it grow */
static const size_t auBucketCounts[] = {509, 1021, 2039, 4093, 8191, 
                                        16381, 32749, 65521};

/* Each key-value pair is stored in a Binding, and points to next Binding */
struct Binding {
    /* The key. */
    const char *key;
     /* The value. */
    void *value;
    /* The address of the next Binding (with same Hash). */
    struct Binding *pNextBinding;
};

/* A SymTable structure symbol table that points to the first Node. */
struct SymTable {
    /* Array of bindings */
    struct Binding **buckets;
    /* Number of bindings */
    size_t size;
    /* Number of buckets*/
    size_t bucketCount;
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

static size_t growHelper(size_t bucketCount){
    const int POSS_BUCKETS = 8;
    int i;
    for (i = 0; i < POSS_BUCKETS - 1; i++){
        if (auBucketCounts[i] == bucketCount)
            return auBucketCounts[i + 1];
    }
    return 0;
}

/*--------------------------------------------------------------------*/

/* Helper function that allocates memory to an SymTable object with 
   bucket count as argument to specify number of buckets. Returns 
   reference to the SymTable object, and NULL if memory not sufficient. */

static SymTable_T newHelper(size_t bucketC){
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

    assert(oSymTable != NULL);
    /* Creates empty newSymTable with larger bucket count */
    uNewBucketCount = growHelper(oldBucketCount);
    newSymTable = newHelper(uNewBucketCount);
    
    if (newSymTable == NULL)
        return 0;
    
    /* Traverses bindings of oSymTable and copies the key-value pairs
       into newSymTable (re-hashed). Frees up bindings during traversal. */
    for (index = 0; index < oldBucketCount; index++){
        struct Binding* currentBind = oSymTable->buckets[index];
        while (currentBind != NULL){
            struct Binding* pCurrent = currentBind;
            SymTable_put(newSymTable, currentBind->key, currentBind->value);
            free((char*)pCurrent->key);
            currentBind = currentBind->pNextBinding;
            free(pCurrent);
        }    
    }
    free(oSymTable->buckets);
    *oSymTable = *newSymTable;
    free(newSymTable);
    return 1;
}

/*--------------------------------------------------------------------*/

SymTable_T SymTable_new(void) {
    const size_t INIT_BUCKET_COUNT = auBucketCounts[0];
    SymTable_T oSymTable = newHelper(INIT_BUCKET_COUNT);

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
        struct Binding* currentBind = oSymTable->buckets[index];
        while (currentBind != NULL){
            struct Binding* pCurrent = currentBind;
            free((char*)pCurrent->key);
            currentBind = currentBind->pNextBinding;
            free(pCurrent);
        }    
    }
    free(oSymTable->buckets);
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
    const size_t MAX_BUCKET_COUNT = auBucketCounts[7];
    int iSuccessful;
    struct Binding* newBinding;
    size_t index;
    assert(oSymTable != NULL);

    if (SymTable_contains(oSymTable, pcKey)) 
        return 0;
    
    /* increase oSymTable bucket count once size reaches current count */
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

    /* create defensive copy */
    newBinding->key = (const char*)malloc(strlen(pcKey) + 1);
    strcpy((char*)newBinding->key, pcKey);
    newBinding->value = (void*) pvValue;
    
    newBinding->pNextBinding = oSymTable->buckets[index];
    oSymTable->buckets[index] = newBinding;
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
    binding = (oSymTable->buckets)[index];

    while (binding != NULL){
        if (strcmp(pcKey, binding->key) == 0){
            void *oldValue = binding->value;
            binding->value = (void*) pvValue; 
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
    binding = oSymTable->buckets[index];

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
    binding = oSymTable->buckets[index];

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
    currBinding = oSymTable->buckets[index];

    /* traverse through bindings and locate it if exists */
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

    /* case where first node is node with key */
    if (prevBinding == NULL)
      oSymTable->buckets[index] = currBinding->pNextBinding;
    else prevBinding->pNextBinding = currBinding->pNextBinding;
   
    returnValue = currBinding->value;
    free((char*) currBinding->key);
    free(currBinding);
    oSymTable->size--;
    return returnValue;
}

/*--------------------------------------------------------------------*/

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra){
   size_t index;
   size_t bucketC;

   assert(oSymTable != NULL);
   assert(pfApply != NULL);
   bucketC = oSymTable->bucketCount;

   for (index = 0; index < bucketC; index++){
        struct Binding* currBinding = oSymTable->buckets[index];
        while (currBinding != NULL){
            (*pfApply)((void*)currBinding->key,(void*)currBinding->value, (void*)pvExtra);
            currBinding = currBinding->pNextBinding;
        }
    }
}
