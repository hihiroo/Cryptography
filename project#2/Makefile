CC=gcc
CFLAGS=-Wall

all: test.o aes.o
	$(CC) $(CFLAGS) -v -o test test.o aes.o -lbsd

test.o: test.c aes.h
	$(CC) $(CFLAGS) -c test.c -lbsd

aes.o: aes.c aes.h
	$(CC) $(CFLAGS) -c aes.c -lbsd

clean:
	rm -rf *.o
	rm -rf test
