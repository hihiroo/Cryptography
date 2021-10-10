CC=gcc
CFLAGS=-Wall

all: test.o aes.o
	$(CC) $(CFLAGS) -o test test.o aes.o

test.o: test.c aes.h
	$(CC) $(CFLAGS) -c test.c

aes.o: aes.c aes.h
	$(CC) $(CFLAGS) -c aes.c

clean:
	rm -rf *.o
	rm -rf test
