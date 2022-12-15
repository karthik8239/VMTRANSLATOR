CC=gcc
CFLAGS=-I.
DEPS = VMTypes.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

VMPagetable: VMTypes.o VMproject.o
	gcc -o VMproject VMPagetable.o VMTypes.o -I.