all: drunkard main

main: main.c
	$(CC) main.c -lcurses -o main
