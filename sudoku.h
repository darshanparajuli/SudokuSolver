#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define DEBUG 0

#define TRUE 1
#define FALSE 0

typedef struct cell_num_pos {
    int x;
    int y;
    int num;
} CellNumPos;

typedef struct stack {
    int index;
    int size;
    CellNumPos *data;
} Stack;


int **board;
int ***possible_numbers;
int size;
int cube_size;

void init_board(void);
int parse_file(const char *path);
void init_possible_numbers(void);
void destroy_board(void);
void destroy_possible_numbers(void);
int is_num_valid(int num, int x, int y);
void calculate_possible_numbers(int num_index, int x, int y);
void calculate_possible_numbers_all(void);
int solve(int x, int y);
int solve_iterative(void);
int is_board_valid(void);
void print_possible_numbers(int x, int y);
void print_possible_numbers_all(void);
void print_board(void);

// stack operations
void init_stack(Stack *stack, int size);
void push_stack_cell_num_pos(Stack *stack, int x, int y, int num);
CellNumPos *pop_stack(Stack *stack);
void destroy_stack(Stack *stack);
