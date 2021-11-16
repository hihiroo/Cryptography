CC=gcc
CFLAGS=-Wall

all: test.o mRSA.o
	$(CC) $(CFLAGS) -o test test.o mRSA.o -lbsd

test.o: test.c mRSA.h
	$(CC) $(CFLAGS) -c test.c -lbsd

mRSA.o: mRSA.c mRSA.h
	$(CC) $(CFLAGS) -c mRSA.c -lbsd

clean:
	rm -rf *.o
	rm -rf test
