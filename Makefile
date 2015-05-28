CC=gcc
CFLAGS=-Wall -O2 -pipe
SOURCES=sudoku.c
EXECUTABLE=solver

$(EXECUTABLE): $(SOURCES)
	$(CC) -o $@ $^ $(CFLAGS)
