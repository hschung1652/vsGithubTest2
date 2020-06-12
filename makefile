all	: tetris.o
	gcc tetris.c -g -lncurses -o main

clean:
	rm a.out *.o
