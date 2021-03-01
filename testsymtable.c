/*--------------------------------------------------------------------*/
/* testsymtable.c                                                     */
/* Author: Bob Dondero                                                */
/*--------------------------------------------------------------------*/

#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>

#ifndef S_SPLINT_S
#include <sys/resource.h>
#endif

/*--------------------------------------------------------------------*/

#define ASSURE(i) assure(i, __LINE__)

/*--------------------------------------------------------------------*/

/* If !iSuccessful, print a message to stdout indicating that the
   test at line iLineNum failed. */

static void assure(int iSuccessful, int iLineNum)
{
   if (! iSuccessful)
   {
      printf("Test at line %d failed.\n", iLineNum);
      fflush(stdout);
   }
}

/*--------------------------------------------------------------------*/

#ifndef S_SPLINT_S
/* Set the process's "CPU time" resource limit.  After the CPU
   time limit expires, the OS will send a SIGKILL signal to the
   process. */

static void setCpuTimeLimit(void)
{
   enum {CPU_TIME_LIMIT_IN_SECONDS = 300};
   struct rlimit sRlimit;
   sRlimit.rlim_cur = CPU_TIME_LIMIT_IN_SECONDS;
   sRlimit.rlim_max = CPU_TIME_LIMIT_IN_SECONDS;
   setrlimit(RLIMIT_CPU, &sRlimit);
}
#endif

/*--------------------------------------------------------------------*/

/* Write the binding whose key is pcKey and whose string value is
   pvValue using format string pvExtra. */

static void printBinding(const char *pcKey, void *pvValue,
   void *pvExtra)
{
   assert(pcKey != NULL);
   assert(pvValue != NULL);
   assert(pvExtra != NULL);

   printf((char*)pvExtra, pcKey, (char*)pvValue);
   fflush(stdout);
}

/*--------------------------------------------------------------------*/

/* Write the binding whose key is pcKey and whose string value is
   pvValue. pvExtra is unused. */

static void printBindingSimple(const char *pcKey, void *pvValue,
   void *pvExtra)
{
   assert(pcKey != NULL);
   assert(pvValue != NULL);
   assert(pvExtra == NULL);

   printf("%s\t%s\n", pcKey, (char*)pvValue);
   fflush(stdout);
}

/*--------------------------------------------------------------------*/

/* Test the most basic SymTable functions. */

static void testBasics(void)
{
   SymTable_T oSymTable;
   char acJeter[] = "Jeter";
   char acMantle[] = "Mantle";
   char acGehrig[] = "Gehrig";
   char acRuth[] = "Ruth";
   char acShortstop[] = "Shortstop";
   char acCenterField[] = "Center Field";
   char acFirstBase[] = "First Base";
   char acRightField[] = "Right Field";

   char acBrown[] = "Brown";
   
   char *pcValue;
   int iSuccessful;
   int iFound;
   size_t uLength;

   printf("------------------------------------------------------\n");
   printf("Testing the most basic SymTable functions.\n");
   printf("No output should appear here:\n");
   fflush(stdout);

   /* Test SymTable_new(). */

   oSymTable = SymTable_new();
   ASSURE(oSymTable != NULL);

   /* Test SymTable_put() and SymTable_getLength(). */

   iSuccessful = SymTable_put(oSymTable, acJeter, acShortstop);
   ASSURE(iSuccessful);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 1);

   iSuccessful = SymTable_put(oSymTable, acMantle, acCenterField);
   ASSURE(iSuccessful);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 2);

   iSuccessful = SymTable_put(oSymTable, acGehrig, acFirstBase);
   ASSURE(iSuccessful);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 3);

   iSuccessful = SymTable_put(oSymTable, acRuth, acRightField);
   ASSURE(iSuccessful);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 4);

   /* Try to insert duplicate to first key entered */
   iSuccessful = SymTable_put(oSymTable, acJeter, acCenterField);
   ASSURE(! iSuccessful);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 4);

   /* Try to insert duplicate to last key entered */
   iSuccessful = SymTable_put(oSymTable, acRuth, acCenterField);
   ASSURE(! iSuccessful);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 4);

   
   /* Test SymTable_contains(). */

   iFound = SymTable_contains(oSymTable, acJeter);
   ASSURE(iFound);

   iFound = SymTable_contains(oSymTable, acMantle);
   ASSURE(iFound);

   iFound = SymTable_contains(oSymTable, acGehrig);
   ASSURE(iFound);

   iFound = SymTable_contains(oSymTable, acRuth);
   ASSURE(iFound);

   iFound = SymTable_contains(oSymTable, "Clemens");
   ASSURE(! iFound);

   iFound = SymTable_contains(oSymTable, "Maris");
   ASSURE(! iFound);

   /* Test SymTable_get(). */

   pcValue = (char*)SymTable_get(oSymTable, acJeter);
   ASSURE(pcValue == acShortstop);

   pcValue = (char*)SymTable_get(oSymTable, acMantle);
   ASSURE(pcValue == acCenterField);

   pcValue = (char*)SymTable_get(oSymTable, acGehrig);
   ASSURE(pcValue == acFirstBase);

   pcValue = (char*)SymTable_get(oSymTable, acRuth);
   ASSURE(pcValue == acRightField);

   pcValue = (char*)SymTable_get(oSymTable, "Clemens");
   ASSURE(pcValue == NULL);

   pcValue = (char*)SymTable_get(oSymTable, "Maris");
   ASSURE(pcValue == NULL);
   
   /* Test SymTable_replace(). */

   pcValue = (char*)
      SymTable_replace(oSymTable, acMantle, acFirstBase);
   ASSURE(pcValue == acCenterField);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 4);

   pcValue = (char*)SymTable_get(oSymTable, acMantle);
   ASSURE(pcValue == acFirstBase);

   pcValue = (char*)
      SymTable_replace(oSymTable, "Clemens", acRightField);
   ASSURE(pcValue == NULL);

   pcValue = (char*)SymTable_replace(oSymTable, "Maris", acRightField);
   ASSURE(pcValue == NULL);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 4);


   /* Insert key with NULL value */
   iSuccessful = SymTable_put(oSymTable, acBrown, NULL);
   ASSURE(iSuccessful);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 5);

   /* Try to insert duplicate of key that had NULL value */
   iSuccessful = SymTable_put(oSymTable, acBrown, acShortstop);
   ASSURE(! iSuccessful);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 5);
 

   /* Test SymTable_free(). */

   SymTable_free(oSymTable);
}

/*--------------------------------------------------------------------*/

/* Test handling of key comparisons. */

static void testKeyComparison(void)
{
   SymTable_T oSymTable;
   char acJeter[] = "Jeter";
   char acJeter2[] = "Jeter";
   char acShortstop[] = "Shortstop";
   char *pcValue;
   int iSuccessful;

   printf("------------------------------------------------------\n");
   printf("Testing key comparison.\n");
   printf("No output should appear here:\n");
   fflush(stdout);

   oSymTable = SymTable_new();
   ASSURE(oSymTable != NULL);
   iSuccessful = SymTable_put(oSymTable, acJeter, acShortstop);
   ASSURE(iSuccessful);
   iSuccessful = SymTable_put(oSymTable, acJeter2, acShortstop);
   ASSURE(! iSuccessful);
   pcValue = (char*)SymTable_get(oSymTable, acJeter2);
   ASSURE(pcValue == acShortstop);
   SymTable_free(oSymTable);
}

/*--------------------------------------------------------------------*/

/* Test handling of key ownership. */

static void testKeyOwnership(void)
{
   enum {MAX_KEY_LENGTH = 10};

   SymTable_T oSymTable;
   char acKey[MAX_KEY_LENGTH];
   char *pcValue;
   int iSuccessful;
   char acCenterField[] = "CenterField";

   printf("------------------------------------------------------\n");
   printf("Testing key ownership.\n");
   printf("No output should appear here:\n");
   fflush(stdout);

   oSymTable = SymTable_new();
   ASSURE(oSymTable != NULL);
   strcpy(acKey, "Mantle");
   iSuccessful = SymTable_put(oSymTable, acKey, acCenterField);
   ASSURE(iSuccessful);
   strcpy(acKey, "xxx");
   pcValue = (char*)SymTable_get(oSymTable, "Mantle");
   ASSURE(pcValue == acCenterField);

   SymTable_free(oSymTable);
}

/*--------------------------------------------------------------------*/

/* Test the SymTable_remove() function. */

static void testRemove(void)
{
   SymTable_T oSymTable;
   char acJeter[] = "Jeter";
   char acMantle[] = "Mantle";
   char acGehrig[] = "Gehrig";
   char acRuth[] = "Ruth";
   char acShortstop[] = "Shortstop";
   char acCenterField[] = "Center Field";
   char acFirstBase[] = "First Base";
   char acRightField[] = "Right Field";

   char *pcValue;
   int iFound;
   size_t uLength;
   int iSuccessful;

   printf("------------------------------------------------------\n");
   printf("Testing the SymTable_remove() function.\n");
   printf("No output should appear here:\n");
   fflush(stdout);

   oSymTable = SymTable_new();
   ASSURE(oSymTable != NULL);

   iSuccessful = SymTable_put(oSymTable, acJeter, acShortstop);
   ASSURE(iSuccessful);

   iSuccessful = SymTable_put(oSymTable, acMantle, acCenterField);
   ASSURE(iSuccessful);

   iSuccessful = SymTable_put(oSymTable, acGehrig, acFirstBase);
   ASSURE(iSuccessful);

   iSuccessful = SymTable_put(oSymTable, acRuth, acRightField);
   ASSURE(iSuccessful);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 4);

   pcValue = (char*)SymTable_remove(oSymTable, acJeter);
   ASSURE(pcValue == acShortstop);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 3);

   iFound = SymTable_contains(oSymTable, acJeter);
   ASSURE(! iFound);

   pcValue = (char*)SymTable_remove(oSymTable, acRuth);
   ASSURE(pcValue == acRightField);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 2);

   iFound = SymTable_contains(oSymTable, acRuth);
   ASSURE(! iFound);

   pcValue = (char*)SymTable_remove(oSymTable, "Clemens");
   ASSURE(pcValue == NULL);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 2);

   iFound = SymTable_contains(oSymTable, "Clemens");
   ASSURE(! iFound);

   pcValue = (char*)SymTable_remove(oSymTable, acRuth);
   ASSURE(pcValue == NULL);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 2);

   iFound = SymTable_contains(oSymTable, acRuth);
   ASSURE(! iFound);

   SymTable_free(oSymTable);
}

/*--------------------------------------------------------------------*/

/* Test the SymTable_map() function. */

static void testMap(void)
{
   SymTable_T oSymTable;
   char acJeter[] = "Jeter";
   char acMantle[] = "Mantle";
   char acGehrig[] = "Gehrig";
   char acRuth[] = "Ruth";
   char acShortstop[] = "Shortstop";
   char acCenterField[] = "Center Field";
   char acFirstBase[] = "First Base";
   char acRightField[] = "Right Field";

   int iSuccessful;

   printf("------------------------------------------------------\n");
   printf("Testing the SymTable_map() function.\n");
   fflush(stdout);

   oSymTable = SymTable_new();
   ASSURE(oSymTable != NULL);

   iSuccessful = SymTable_put(oSymTable, acJeter, acShortstop);
   ASSURE(iSuccessful);

   iSuccessful = SymTable_put(oSymTable, acMantle, acCenterField);
   ASSURE(iSuccessful);

   iSuccessful = SymTable_put(oSymTable, acGehrig, acFirstBase);
   ASSURE(iSuccessful);

   iSuccessful = SymTable_put(oSymTable, acRuth, acRightField);
   ASSURE(iSuccessful);

   printf("Four players and their positions should appear here:\n");
   fflush(stdout);
   SymTable_map(oSymTable, printBinding, "%s\t%s\n");

   printf("Four players and their positions should appear here:\n");
   fflush(stdout);
   SymTable_map(oSymTable, printBindingSimple, NULL);

   SymTable_free(oSymTable);
}

/*--------------------------------------------------------------------*/

/* Test a SymTable object that contains no bindings. */

static void testEmptyTable(void)
{
   SymTable_T oSymTable;
   char *pcValue;
   int iFound;
   size_t uLength;

   printf("------------------------------------------------------\n");
   printf("Testing an empty SymTable object.\n");
   printf("No output should appear here:\n");
   fflush(stdout);

   oSymTable = SymTable_new();
   ASSURE(oSymTable != NULL);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 0);

   iFound = SymTable_contains(oSymTable, "Jeter");
   ASSURE(! iFound);

   pcValue = (char*)SymTable_get(oSymTable, "Jeter");
   ASSURE(pcValue == NULL);

   pcValue = (char*)SymTable_remove(oSymTable, "Jeter");
   ASSURE(pcValue == NULL);

   SymTable_map(oSymTable, printBinding, "%s\t%s\n");

   SymTable_free(oSymTable);
}

/*--------------------------------------------------------------------*/

/* Test the ability of a SymTable object to contain an empty key. */

static void testEmptyKey(void)
{
   SymTable_T oSymTable;
   char *pcValue;
   char acShortstop[] = "Shortstop";
   int iFound;
   size_t uLength;
   int iSuccessful;

   printf("------------------------------------------------------\n");
   printf("Testing a SymTable object that contains an empty key.\n");
   fflush(stdout);

   oSymTable = SymTable_new();
   ASSURE(oSymTable != NULL);

   iSuccessful = SymTable_put(oSymTable, "", acShortstop);
   ASSURE(iSuccessful);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 1);

   iFound = SymTable_contains(oSymTable, "");
   ASSURE(iFound);

   pcValue = (char*)SymTable_get(oSymTable, "");
   ASSURE(pcValue == acShortstop);

   printf("An empty name and a position should appear here:\n");
   fflush(stdout);
   SymTable_map(oSymTable, printBinding, "%s\t%s\n");

   pcValue = (char*)SymTable_remove(oSymTable, "");
   ASSURE(pcValue == acShortstop);

   SymTable_free(oSymTable);
}

/*--------------------------------------------------------------------*/

/* Test the ability of a SymTable object to contain NULL values. */

static void testNullValue(void)
{
   SymTable_T oSymTable;
   char *pcValue;
   int iFound;
   size_t uLength;
   int iSuccessful;
   char acCenterField[] = "Center Field";

   printf("------------------------------------------------------\n");
   printf("Testing a SymTable object that contains NULL values.\n");
   printf("No output should appear here:\n");
   fflush(stdout);

   oSymTable = SymTable_new();
   ASSURE(oSymTable != NULL);

   iSuccessful = SymTable_put(oSymTable, "Jeter", NULL);
   ASSURE(iSuccessful);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 1);

   iFound = SymTable_contains(oSymTable, "Jeter");
   ASSURE(iFound);

   pcValue = (char*)SymTable_get(oSymTable, "Jeter");
   ASSURE(pcValue == NULL);

   pcValue = (char*)SymTable_remove(oSymTable, "Jeter");
   ASSURE(pcValue == NULL);
   
   iSuccessful = SymTable_put(oSymTable, "Mantle", acCenterField);
   ASSURE(iSuccessful);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 1);

   iFound = SymTable_contains(oSymTable, "Mantle");
   ASSURE(iFound);

   pcValue = (char*)SymTable_get(oSymTable, "Mantle");
   ASSURE(pcValue == acCenterField);

   pcValue = SymTable_replace(oSymTable, "Mantle", NULL);
   ASSURE(pcValue == acCenterField);

   iFound = SymTable_contains(oSymTable, "Mantle");
   ASSURE(iFound);
   
   pcValue = (char*)SymTable_get(oSymTable, "Mantle");
   ASSURE(pcValue == NULL);

   pcValue = (char*)SymTable_remove(oSymTable, "Mantle");
   ASSURE(pcValue == NULL);   

   SymTable_free(oSymTable);
}

/*--------------------------------------------------------------------*/

/* Test the ability of a SymTable object to contain long keys. */

static void testLongKey(void)
{
   enum {KEY_SIZE = 1000};

   SymTable_T oSymTable;
   char acKeyA[KEY_SIZE];
   char acKeyB[KEY_SIZE];
   char *pcValue;
   char acShortstop[] = "Shortstop";
   int iFound;
   int i;
   size_t uLength;
   int iSuccessful;

   printf("------------------------------------------------------\n");
   printf("Testing a SymTable object that contains long keys.\n");
   printf("No output should appear here:\n");
   fflush(stdout);

   for (i = 0; i < KEY_SIZE - 1; i++)
      acKeyA[i] = 'a';
   acKeyA[KEY_SIZE - 1] = '\0';

   for (i = 0; i < KEY_SIZE - 1; i++)
      acKeyB[i] = 'b';
   acKeyB[KEY_SIZE - 1] = '\0';

   oSymTable = SymTable_new();
   ASSURE(oSymTable != NULL);

   iSuccessful = SymTable_put(oSymTable, acKeyA, acShortstop);
   ASSURE(iSuccessful);
   iSuccessful = SymTable_put(oSymTable, acKeyB, acShortstop);
   ASSURE(iSuccessful);
   iSuccessful = SymTable_put(oSymTable, acKeyB, acShortstop);
   ASSURE(! iSuccessful);

   uLength = SymTable_getLength(oSymTable);
   ASSURE(uLength == 2);

   iFound = SymTable_contains(oSymTable, acKeyA);
   ASSURE(iFound);
   iFound = SymTable_contains(oSymTable, acKeyB);
   ASSURE(iFound);

   pcValue = (char*)SymTable_get(oSymTable, acKeyA);
   ASSURE(pcValue == acShortstop);
   pcValue = (char*)SymTable_get(oSymTable, acKeyB);
   ASSURE(pcValue == acShortstop);

   pcValue = (char*)SymTable_remove(oSymTable, acKeyB);
   ASSURE(pcValue == acShortstop);
   pcValue = (char*)SymTable_remove(oSymTable, acKeyB);
   ASSURE(pcValue == NULL);
   pcValue = (char*)SymTable_remove(oSymTable, acKeyA);
   ASSURE(pcValue == acShortstop);

   SymTable_free(oSymTable);
}

/*--------------------------------------------------------------------*/

/* Test the ability of SymTable object to have values that are
   other SymTable objects. */

static void testTableOfTables(void)
{
   SymTable_T oSymTable;
   SymTable_T oSymTable1;
   SymTable_T oSymTable2;
   SymTable_T oSymTableRet;
   char acShortstop[] = "Shortstop";
   char acCenterField[] = "CenterField";
   char *pcValue;
   int iSuccessful;

   printf("------------------------------------------------------\n");
   printf("Testing a SymTable object that contains other\n");
   printf("SymTable objects.\n");
   printf("No output should appear here:\n");
   fflush(stdout);

   oSymTable1 = SymTable_new();
   ASSURE(oSymTable1 != NULL);

   iSuccessful = SymTable_put(oSymTable1, "Jeter", acShortstop);
   ASSURE(iSuccessful);

   oSymTable2 = SymTable_new();
   ASSURE(oSymTable2 != NULL);

   iSuccessful = SymTable_put(oSymTable2, "Mantle", acCenterField);
   ASSURE(iSuccessful);

   oSymTable = SymTable_new();
   ASSURE(oSymTable != NULL);

   iSuccessful = SymTable_put(oSymTable, "first table", oSymTable1);
   ASSURE(iSuccessful);

   iSuccessful = SymTable_put(oSymTable, "second table", oSymTable2);
   ASSURE(iSuccessful);

   oSymTableRet = (SymTable_T)SymTable_get(oSymTable, "first table");
   ASSURE(oSymTableRet == oSymTable1);

   pcValue = (char*)SymTable_get(oSymTableRet, "Jeter");
   ASSURE(pcValue == acShortstop);

   oSymTableRet = (SymTable_T)SymTable_get(oSymTable, "second table");
   ASSURE(oSymTableRet == oSymTable2);

   pcValue = (char*)SymTable_get(oSymTableRet, "Mantle");
   ASSURE(pcValue == acCenterField);

   SymTable_free(oSymTable2);
   SymTable_free(oSymTable1);
   SymTable_free(oSymTable);
}

/*--------------------------------------------------------------------*/

/* Test the ability of a SymTable object to handle collisions.  This
   test assumes that a SymTable object is implemented as a hash table,
   that there are 509 buckets in the hash table, and that the
   implementation uses the hash function provided in the assignment
   specification. */

static void testCollisions(void)
{
   SymTable_T oSymTable;
   int iSuccessful;
   char acCenterField[] = "pitcher";
   char acCatcher[] = "catcher";
   char acFirstBase[] = "first base";
   char acRightField[] = "second base";
   char *pcValue;

   printf("------------------------------------------------------\n");
   printf("Testing the collision handling of a SymTable object\n");
   printf("assuming a hash table implementation, and assuming that\n");
   printf("the implementation uses the hash function from the\n");
   printf("assignment specification.\n");
   printf("No output should appear here:\n");
   fflush(stdout);

   oSymTable = SymTable_new();
   ASSURE(oSymTable != NULL);

   /* Note that strings "250", "469", "947", "1303", and "2016" hash
      to the same bucket -- bucket 123. */

   iSuccessful = SymTable_put(oSymTable, "250", acCenterField);
   ASSURE(iSuccessful);

   iSuccessful = SymTable_put(oSymTable, "469", acCatcher);
   ASSURE(iSuccessful);

   iSuccessful = SymTable_put(oSymTable, "947", acFirstBase);
   ASSURE(iSuccessful);

   iSuccessful = SymTable_put(oSymTable, "1303", acRightField);
   ASSURE(iSuccessful);

   iSuccessful = SymTable_put(oSymTable, "2016", acRightField);
   ASSURE(iSuccessful);

   pcValue = SymTable_get(oSymTable, "250");
   ASSURE(pcValue == acCenterField);

   pcValue = SymTable_get(oSymTable, "469");
   ASSURE(pcValue == acCatcher);

   pcValue = SymTable_get(oSymTable, "947");
   ASSURE(pcValue == acFirstBase);

   pcValue = SymTable_get(oSymTable, "1303");
   ASSURE(pcValue == acRightField);

   pcValue = SymTable_get(oSymTable, "2016");
   ASSURE(pcValue == acRightField);

   pcValue = SymTable_remove(oSymTable, "947");
   ASSURE(pcValue == acFirstBase);

   pcValue = SymTable_remove(oSymTable, "2016");
   ASSURE(pcValue == acRightField);

   pcValue = SymTable_remove(oSymTable, "250");
   ASSURE(pcValue == acCenterField);

   pcValue = SymTable_get(oSymTable, "469");
   ASSURE(pcValue == acCatcher);

   pcValue = SymTable_get(oSymTable, "1303");
   ASSURE(pcValue == acRightField);

   SymTable_free(oSymTable);
}

/*--------------------------------------------------------------------*/

/* Test the ability of a SymTable object to be large, that is, to
   contain iBindingCount bindings. Write the time consumed to stdout. */

static void testLargeTable(int iBindingCount)
{
   enum {MAX_KEY_LENGTH = 10};

   SymTable_T oSymTable;
   SymTable_T oSymTableSmall;
   char acKey[MAX_KEY_LENGTH];
   char *pcValue;
   int i;
   int iSmall;
   int iLarge;
   int iSuccessful;
   clock_t iInitialClock;
   clock_t iFinalClock;
   size_t uLength = 0;
   size_t uLength2;

   printf("------------------------------------------------------\n");
   printf("Testing a potentially large SymTable object.\n");
   printf("No output except CPU time consumed should appear here:\n");
   fflush(stdout);

   /* Note the current time. */
   iInitialClock = clock();

   /* Create oSymTableSmall, and put a couple of bindings into it. */
   oSymTableSmall = SymTable_new();
   ASSURE(oSymTableSmall != NULL);
   iSuccessful = SymTable_put(oSymTableSmall, "xxx", "xxx");
   ASSURE(iSuccessful);
   iSuccessful = SymTable_put(oSymTableSmall, "yyy", "yyy");
   ASSURE(iSuccessful);

   /* Create oSymTable, the primary SymTable object. */
   oSymTable = SymTable_new();
   ASSURE(oSymTable != NULL);

   /* Put iBindingCount new bindings into oSymTable.  Each binding's
      key and value contain the same characters. */
   for (i = 0; i < iBindingCount; i++)
   {
      sprintf(acKey, "%d", i);
      pcValue = (char*)malloc(sizeof(char) * (strlen(acKey) + 1));
      ASSURE(pcValue != NULL);
      strcpy(pcValue, acKey);
      iSuccessful = SymTable_put(oSymTable, acKey, pcValue);
      ASSURE(iSuccessful);
      uLength = SymTable_getLength(oSymTable);
      ASSURE(uLength == (size_t)(i+1));
   }

   /* Get each binding's value, and make sure that it contains
      the same characters as its key. */
   iSmall = 0;
   iLarge = iBindingCount - 1;
   while (iSmall < iLarge)
   {
      /* Get the smallest of the remaining bindings. */
      sprintf(acKey, "%d", iSmall);
      pcValue = (char*)SymTable_get(oSymTable, acKey);
      ASSURE(pcValue != NULL);
      ASSURE((pcValue != NULL) && (strcmp(pcValue, acKey) == 0));
      iSmall++;
      /* Get the largest of the remaining bindings. */
      sprintf(acKey, "%d", iLarge);
      pcValue = (char*)SymTable_get(oSymTable, acKey);
      ASSURE(pcValue != NULL);
      ASSURE((pcValue != NULL) && (strcmp(pcValue, acKey) == 0));
      iLarge--;
   }
   /* Get the middle binding -- if there is one. */
   if (iSmall == iLarge)
   {
      sprintf(acKey, "%d", iSmall);
      pcValue = (char*)SymTable_get(oSymTable, acKey);
      ASSURE(pcValue != NULL);
      ASSURE((pcValue != NULL) && (strcmp(pcValue, acKey) == 0));
   }

   /* Remove each binding. Also free each binding's value. */
   iSmall = 0;
   iLarge = iBindingCount - 1;
   while (iSmall < iLarge)
   {
      /* Remove the smallest of the remaining bindings. */
      sprintf(acKey, "%d", iSmall);
      pcValue = (char*)SymTable_remove(oSymTable, acKey);
      ASSURE(pcValue != NULL);
      ASSURE((pcValue != NULL) && (strcmp(pcValue, acKey) == 0));
      free(pcValue);
      uLength--;
      uLength2 = SymTable_getLength(oSymTable);
      ASSURE(uLength2 == uLength);
      iSmall++;
      /* Remove the largest of the remaining bindings. */
      sprintf(acKey, "%d", iLarge);
      pcValue = (char*)SymTable_remove(oSymTable, acKey);
      ASSURE(pcValue != NULL);
      ASSURE((pcValue != NULL) && (strcmp(pcValue, acKey) == 0));
      free(pcValue);
      uLength--;
      uLength2 = SymTable_getLength(oSymTable);
      ASSURE(uLength2 == uLength);
      iLarge--;
   }
   /* Remove the middle binding -- if there is one. */
   if (iSmall == iLarge)
   {
      sprintf(acKey, "%d", iSmall);
      pcValue = (char*)SymTable_remove(oSymTable, acKey);
      ASSURE(pcValue != NULL);
      ASSURE((pcValue != NULL) && (strcmp(pcValue, acKey) == 0));
      free(pcValue);
      uLength--;
      uLength2 = SymTable_getLength(oSymTable);
      ASSURE(uLength2 == uLength);  
   }

   /* Make sure oSymTableSmall hasn't been corrupted by expansion
      of oSymTable. */
   pcValue = (char*)SymTable_get(oSymTableSmall, "xxx");
   ASSURE((pcValue != NULL) && (strcmp(pcValue, "xxx") == 0));
   pcValue = (char*)SymTable_get(oSymTableSmall, "yyy");
   ASSURE((pcValue != NULL) && (strcmp(pcValue, "yyy") == 0));

   /* Free both SymTable objects. */
   SymTable_free(oSymTable);
   SymTable_free(oSymTableSmall);

   /* Note the current time, and print the time consumed to stdout. */
   iFinalClock = clock();
   printf("CPU time (%d bindings):  %f seconds\n", iBindingCount,
      ((double)(iFinalClock - iInitialClock)) / CLOCKS_PER_SEC);
   fflush(stdout);
}

/*--------------------------------------------------------------------*/

/* Test the SymTable ADT.  Write the output of the tests to stdout.
   As always, argc is the command-line argument count, argv contains
   the command-line arguments, and argv[0] is the name of the
   executable binary file. argv[1] is the number of bindings to put
   into a potentially large SymTable object.  Exit with EXIT_FAILURE
   if argv[1] is missing or not numeric.  Otherwise return 0. */

int main(int argc, char *argv[])
{
   int iBindingCount;

   if (argc != 2)
   {
      fprintf(stderr, "Usage: %s bindingcount\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   if (sscanf(argv[1], "%d", &iBindingCount) != 1)
   {
      fprintf(stderr, "bindingcount must be numeric\n");
      exit(EXIT_FAILURE);
   }
   if (iBindingCount < 0)
   {
      fprintf(stderr, "bindingcount cannot be negative\n");
      exit(EXIT_FAILURE);
   }
   
#ifndef S_SPLINT_S
   setCpuTimeLimit();
#endif

   testBasics();
   testKeyComparison();
   testKeyOwnership();
   testRemove();
   testMap();
   testEmptyTable();
   testEmptyKey();
   testNullValue();
   testLongKey();
   testTableOfTables();
   testCollisions();
   testLargeTable(iBindingCount);

   printf("------------------------------------------------------\n");
   printf("End of %s.\n", argv[0]);
   return 0;
}
