all: main

main: main.c cavegen.o
	$(CC) main.c cavegen.o -lcurses -o main -D_XOPEN_SOURCE_EXTENDED

cavegen.o: cavegen.c
	$(CC) cavegen.c -c -o cavegen.o

clean:
	rm -rf main cavegen.o
