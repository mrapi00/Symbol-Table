/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: Mahmudul Rapi                                              */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "symtable.h"

/*--------------------------------------------------------------------*/

/* Each item is stored in a node */

struct Node
{
   /* The key. */
   const char *key;
   /* The value. */
   void* value;
   /* The address of the next Node. */
   struct Node *psNextNode;
};

/*--------------------------------------------------------------------*/

/* A SymTable structure symbole table that points to the first Node. */

struct SymTable
{
   /* size of symtable */
   size_t size;
   /* The address of the first node. */
   struct Node *psFirstNode;
};

/*--------------------------------------------------------------------*/

SymTable_T SymTable_new(void){
   SymTable_T oSymTable;
   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (oSymTable == NULL)
      return NULL;

   oSymTable->psFirstNode = NULL;
   oSymTable->size = 0;
   return oSymTable;
}

/*--------------------------------------------------------------------*/

void SymTable_free(SymTable_T oSymTable){
   struct Node *psCurrentNode;
   struct Node *psNextNode;

   assert(oSymTable != NULL);

   for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psNextNode)
   {
      psNextNode = psCurrentNode->psNextNode;
      free(psCurrentNode);
   }

   free(oSymTable);
}

/*--------------------------------------------------------------------*/

size_t SymTable_getLength(SymTable_T oSymTable){
   return oSymTable->size;
}

/*--------------------------------------------------------------------*/

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue){
   struct Node *psNewNode;

   assert(oSymTable != NULL);

   if (SymTable_contains(oSymTable, pcKey)) 
        return 0;

   psNewNode = (struct Node*)malloc(sizeof(struct Node));
   if (psNewNode == NULL)
      return 0;

   psNewNode->key = (const char*)malloc(strlen(pcKey) + 1);
   strcpy((char*)psNewNode->key, pcKey);
   psNewNode->value = (void*) pvValue;

   psNewNode->psNextNode = oSymTable->psFirstNode;
   oSymTable->psFirstNode = psNewNode;
   oSymTable->size++;
   return 1;
}

/*--------------------------------------------------------------------*/

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
    const void *pvValue){
      struct Node* current;
      assert(oSymTable != NULL);

      current = oSymTable->psFirstNode;
      while (current != NULL){
         if (strcmp(pcKey, current->key) == 0){
            void *oldValue = current->value;
            current->value = (void*) pvValue;
            return oldValue; 
         }
         current = current->psNextNode;
      }
      return NULL;
    }

/*--------------------------------------------------------------------*/

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
   struct Node* current;
   assert(oSymTable != NULL);

   current = oSymTable->psFirstNode;
   while (current != NULL){
      if (strcmp(pcKey, current->key) == 0){
         return 1; 
      }
      current = current->psNextNode;
   }
   return 0;
}

/*--------------------------------------------------------------------*/

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
   struct Node* current;
   assert(oSymTable != NULL);

   current = oSymTable->psFirstNode;
   while (current != NULL){
      if (strcmp(pcKey, current->key) == 0){
         return current->value; 
      }
      current = current->psNextNode;
   }
   return NULL;
}

/*--------------------------------------------------------------------*/
 
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
   struct Node* current;
   struct Node* prevNode = NULL;
   void *returnValue;
   bool found = false;
   assert(oSymTable != NULL);

   current = oSymTable->psFirstNode;
   while (current != NULL){
      if (strcmp(pcKey, current->key) == 0){
         found = true;
         break;
      }
      prevNode = current;
      current = current->psNextNode;
   }
   if (!found)
      return NULL;
   /* case where first node is node with key */
   if (prevNode == NULL)
      oSymTable->psFirstNode = current->psNextNode;
   else prevNode->psNextNode = current->psNextNode;
   
   returnValue = current->value;
   free((char*) current->key);
   free(current);
   oSymTable->size--;
   return returnValue;
}

/*--------------------------------------------------------------------*/

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra);
