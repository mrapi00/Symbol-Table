#Macros
CC = gcc217
# CC = gcc217m
CFLAGS =
#CFLAGS = -g

# Dependency rules for non-file targets
all: testsymtablelist testsymtablehash

clobber: clean
	rm -f *~ \#*\#

clean:
	rm -f testsymtablelist testsymtablehash *.o

# Dependency rules for file targets
testsymtablelist: testsymtable.o symtablelist.o
	$(CC) $(CFLAGS) testsymtable.o symtablelist.o -o testsymtablelist

testsymtablehash: testsymtable.o symtablehash.o
	$(CC) $(CFLAGS) testsymtable.o symtablehash.o -o testsymtablehash

testsymtable.o: testsymtable.c symtable.h
	$(CC) $(CFLAGS) -c testsymtable.c

symtablelist.o: symtablelist.c symtable.h
	$(CC) $(CFLAGS) -c symtablelist.c

symtablehash.o: symtablehash.c symtable.h
	$(CC) $(CFLAGS) -c symtablehash.c
