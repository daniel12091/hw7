# These are variables. We usually define CC, CCLINK, CFLAGS, and OBJS
CC=gcc
CCLINK=$(CC) # Read variable CC using $(CC)
CFLAGS=-g -Wall -std=c99
OBJS=grades.o libgrades.so

# The first rule is invoked by default.
# We can also invoke it (or any other rule) manually with "make all" - 
# note that "all" is the rule's name!
libgrades.so: grades.o
	$(CCLINK) -shared grades.o -o libgrades.so -llinked-list -L.

grades.o: grades.c grades.h
	$(CC) $(CFLAGS) -c -fpic grades.c

# Custom rule - we can invoke it manually by calling "make clean"
# We usually have a "clean" rule to delete any automatically generated files
clean:
	rm -rf *.o libgrades.so