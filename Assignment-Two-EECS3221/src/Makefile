#makefile for resource manager program
CC=gcc

DEPS = Task.h Resource.h Activity.h Bankers.h queue.h

OBJ = banker.o queue.o

CFLAGS=-Wall

STD_FLAG=-std=c99


banker.o: banker.c Bankers.h Task.h Resource.h Activity.h

	$(CC) $(CFLAGS) banker.c queue.c $(STD_FLAG)  -o Banker
queue.o: queue.c queue.h Activity.h
	$(CC) $(CFLAGS) -c queue.c

clean:

	rm -rf *.o    
