CC=gcc
CFLAGS=-Wall -march=corei7-avx -O2 -pipe
SOURCES=sudoku.c
EXECUTABLE=sudoku

$(EXECUTABLE): $(SOURCES)
	gcc -o $@ $^ $(CFLAGS)
