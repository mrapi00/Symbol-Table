/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: Mahmudul Rapi                                              */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "symtable.h"

/*--------------------------------------------------------------------*/

/* Each key-value pair is stored in a node, and points to next Node */
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

/* A SymTable structure symbol table implemented as a linked list and  
 maintains its size (number of Nodes) */

struct SymTable
{
   /* The size (node count) of SymTable */
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
      free((char*)psCurrentNode->key);
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
   assert(pcKey != NULL);

   /* traverse list just once to check if pcKey already in the SymTable */
   if (SymTable_contains(oSymTable, pcKey)) 
      return 0;
   
   psNewNode = (struct Node*)malloc(sizeof(struct Node));
   if (psNewNode == NULL)
      return 0;
   
   /* create a defensive copy of key */
   psNewNode->key = (const char*)malloc(strlen(pcKey) + 1);
   strcpy((char*)psNewNode->key, pcKey);
   psNewNode->value = (void*) pvValue;

   /* append new Node to beginning of the list (since we know pcKey 
      not already in SymTable so no additional traversal needed) */
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
      assert(pcKey != NULL);

      /* traverse list until finding pcKey and replace */
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
   assert(pcKey != NULL);
   
   /* traverse list until finding pcKey and return 1 if found */
   current = oSymTable->psFirstNode;
   while (current != NULL){
      if (strcmp(pcKey, current->key) == 0)
         return 1; 
      current = current->psNextNode;
   }
   return 0;
}

/*--------------------------------------------------------------------*/

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
   struct Node* current;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /* traverse list until finding pcKey and return its value */
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
   int found = 0;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /* traverse thru Nodes until found */
   current = oSymTable->psFirstNode;
   while (current != NULL){
      if (strcmp(pcKey, current->key) == 0){
         found = 1;
         break;
      }
      prevNode = current;
      current = current->psNextNode;
   }
   if (!found || current == NULL)
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
    const void *pvExtra){

   struct Node *psCurrentNode;
   assert(oSymTable != NULL);
   assert(pfApply != NULL);

   /* traverse list and apply pfApply to all key-value pairs */
   for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psCurrentNode->psNextNode)
      (*pfApply)((void*)psCurrentNode->key,(void*)psCurrentNode->value, (void*)pvExtra);
    }
