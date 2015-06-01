CC=gcc
CFLAGS=-Wall -O2 -pipe
SOURCES=main.c sudoku.h sudoku.c
EXECUTABLE=solver

$(EXECUTABLE): $(SOURCES)
	$(CC) -o $@ $^ $(CFLAGS)
