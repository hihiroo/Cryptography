CC=gcc
CFLAGS=-Wall

all: test.o miller_rabin.o mod.o
	$(CC) $(CFLAGS) -o test test.o miller_rabin.o mod.o

test.o: test.c miller_rabin.h
	$(CC) $(CFLAGS) -c test.c

miller_rabin.o: miller_rabin.c miller_rabin.h
	$(CC) $(CFLAGS) -c miller_rabin.c

mod.o: mod.c miller_rabin.h
	$(CC) $(CFLAGS) -c mod.c

clean:
	rm -rf *.o
	rm -rf test
