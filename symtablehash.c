/*--------------------------------------------------------------------*/
/* symtablehash.c                                                     */
/* Author: Mahmudul Rapi                                              */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
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

/* A SymTable structure symbol table implemented with hash buckets that
   contain bindings. */
struct SymTable {
    /* Array of bindings */
    struct Binding **buckets;
    /* The size (number of bindings) in SymTable */
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
   based on current bucketC number of buckets, and returns the value. */

static size_t SymTable_growHelper(size_t bucketC){
    const int POSS_BUCKETS = 8;
    int i;
    for (i = 0; i < POSS_BUCKETS - 1; i++){
        if (auBucketCounts[i] == bucketC)
            return auBucketCounts[i + 1];
    }
    return 0;
}

/*--------------------------------------------------------------------*/

/* Helper function that allocates memory to an SymTable object with 
   bucketC number of buckets. Returns reference to the SymTable object, 
   and NULL if memory not sufficient. */

static SymTable_T SymTable_newHelper(size_t bucketC){
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

/* Increase the bucket count of oSymTable by allocated new larger memory
   and transferring over key-value pairs. Frees up old memory (smaller
   SymTable).  Return 1 (TRUE) ifsuccessful, or 0 (FALSE) if insufficient 
   memory is available. */

static int SymTable_grow(SymTable_T oSymTable)
{
    size_t index;
    size_t uNewBucketCount;
    size_t oldBucketCount = oSymTable->bucketCount;
    SymTable_T newSymTable;

    assert(oSymTable != NULL);
    /* Creates empty newSymTable with larger bucket count */
    uNewBucketCount = SymTable_growHelper(oldBucketCount);
    newSymTable = SymTable_newHelper(uNewBucketCount);
    
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
    /* Create a SymTable of the default bucket size */
    SymTable_T oSymTable = SymTable_newHelper(INIT_BUCKET_COUNT);

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
    int resized = 0; /* 0 if SymTable not grown, 1 if grown */
    struct Binding* binding;
    struct Binding* newBinding;
    size_t index;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    
    index = SymTable_hash(pcKey, oSymTable->bucketCount);
    binding = oSymTable->buckets[index];
    /* traverse corresponding bucket until finding pcKey and return 0
       if found */
    while (binding != NULL){
        if (strcmp(pcKey, binding->key) == 0)
            return 0;
        binding = binding->pNextBinding;
    }
    
    /* Increase oSymTable bucket count once its size reaches 
       current bucketCount */
    
    if (oSymTable->size == oSymTable->bucketCount && 
        oSymTable->bucketCount != MAX_BUCKET_COUNT)
    {
       iSuccessful = SymTable_grow(oSymTable);
       resized = 1;
       if (!iSuccessful)
          return 0;
    }
    /* only compute hash again if SymTable has changed */
    if (resized)
        index = SymTable_hash(pcKey, oSymTable->bucketCount);
    
    newBinding = (struct Binding*)malloc(sizeof(struct Binding));
    if (newBinding == NULL)
        return 0;

    /* create defensive copy */
    newBinding->key = (const char*)malloc(strlen(pcKey) + 1);
    strcpy((char*)newBinding->key, pcKey);
    newBinding->value = (void*) pvValue;
    
    /* append new binding to beginning of hash bucket (since we know pcKey 
       not already in SymTable so no additional traversal needed) */
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
    assert(pcKey != NULL);

    index = SymTable_hash(pcKey, oSymTable->bucketCount);
    binding = (oSymTable->buckets)[index];

    /* traverse corresponding bucket until finding pcKey and replace */
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
    assert(pcKey != NULL);

    index = SymTable_hash(pcKey, oSymTable->bucketCount);
    binding = oSymTable->buckets[index];

    /* traverse corresponding bucket until finding pcKey and return 1
       if found */
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
    assert(pcKey != NULL);

    index = SymTable_hash(pcKey, oSymTable->bucketCount);
    binding = oSymTable->buckets[index];

    /* traverse corresponding bucket until finding pcKey and return 1
       if found */
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
    int found = 0;
    struct Binding* currBinding = NULL;
    struct Binding* prevBinding = NULL;
    void *returnValue;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    
    index = SymTable_hash(pcKey, oSymTable->bucketCount);
    if (oSymTable->buckets[index] == NULL)
        return NULL;
    else currBinding = oSymTable->buckets[index];
    
    /* traverse through bindings and locate it if exists */
    while (currBinding != NULL){
        if (strcmp(pcKey, currBinding->key) == 0){
            found = 1;
            break;
        }
        prevBinding = currBinding;
        currBinding = currBinding->pNextBinding;
    }

    if (!found || currBinding == NULL)
        return NULL;

    /* case where first binding is binding with key */
    if (prevBinding == NULL){
        if (currBinding == NULL)
            oSymTable->buckets[index] = NULL;
        else oSymTable->buckets[index] = currBinding->pNextBinding;
    }
    else {
        if (currBinding == NULL)
            prevBinding->pNextBinding  = NULL;
        else prevBinding->pNextBinding = currBinding->pNextBinding;
    }

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
   
   /* traverse all bindings and apply pfApply to all key-value pairs */
   for (index = 0; index < bucketC; index++){
        struct Binding* currBinding = oSymTable->buckets[index];
        while (currBinding != NULL){
            (*pfApply)((void*)currBinding->key,(void*)currBinding->value, (void*)pvExtra);
            currBinding = currBinding->pNextBinding;
        }
    }
}
