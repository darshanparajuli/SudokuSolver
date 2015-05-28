CC=gcc
CFLAGS=-Wall -O2 -pipe
SOURCES=sudoku.c
EXECUTABLE=sudoku

$(EXECUTABLE): $(SOURCES)
	gcc -o $@ $^ $(CFLAGS)
