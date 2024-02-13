all: main

CFLAGS=-g

main: main.c cavegen.o ht.o
	$(CC) main.c cavegen.o ht.o -lcurses -o main -D_XOPEN_SOURCE_EXTENDED $(CFLAGS)

cavegen.o: cavegen.c
	$(CC) cavegen.c -c -o cavegen.o $(CFLAGS)

ht.o: ht.c
	$(CC) ht.c -c -o ht.o $(CFLAGS)

clean:
	rm -rf main cavegen.o
