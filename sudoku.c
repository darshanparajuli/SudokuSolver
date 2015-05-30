#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define DEBUG 0

#define TRUE 1
#define FALSE 0

struct cell_num_pos {
    int x;
    int y;
    int num;
    struct cell_num_pos *next;
};

typedef struct stack {
    struct cell_num_pos *top;
} Stack;

static int **board;
static int ***possible_numbers;
static int size;
static int cube_size;

#if DEBUG
static struct timespec sleep_time;
static int is_num_valid_counter = 0;
#endif

void init_board(void);
void parse_file(const char *);
void init_possible_numbers(void);
void destroy_board(void);
void destroy_possible_numbers(void);
int is_num_valid(int, int, int);
void calculate_possible_numbers(int, int, int);
void calculate_possible_numbers_all(void);
int solve(int, int);
int solve_iterative(void);
int is_board_valid(void);
void print_possible_numbers(int, int);
void print_possible_numbers_all(void);
void print_board(void);

void push_stack_cell_num_pos(Stack *, int, int, int);
struct cell_num_pos * pop_stack(Stack *);
void destroy_stack(Stack *);

int main(int argc, char **argv) {
    struct timespec time;
    double beforeTime, afterTime;
    const char* path;

    if (argc <= 1) {
        printf("Error: missing path as argument!\n");
        return EXIT_FAILURE;
    }

    path = argv[1];
    
    printf("Initializing...\n");
    parse_file(path);

    printf("Board: \n");
    print_board();

    init_possible_numbers();
    calculate_possible_numbers_all();
    
#if DEBUG
    sleep_time.tv_nsec = 500000;
#endif
    
    printf("Solving...\n");
    clock_gettime(CLOCK_REALTIME, &time);
    beforeTime = time.tv_sec + (time.tv_nsec / 1000000000.0);

    // int result = solve(0, 0);
    int result = solve_iterative();
    
    clock_gettime(CLOCK_REALTIME, &time);
    afterTime = time.tv_sec + (time.tv_nsec / 1000000000.0);

#if DEBUG
    printf("is_num_valid calls: %d\n", is_num_valid_counter);
#endif
    
    if (result) {
        printf("Solved! ");
    } else {
        printf("Couldn't solve. ");
    }

    if (is_board_valid()) {
        printf("Board is valid.\n");
    } else {
        printf("Board is NOT valid.\n");
    }

    print_board();

    printf("Time taken: %.6f seconds\n", (afterTime - beforeTime));
    printf("Cleaning up...\n");
    
    destroy_board();
    destroy_possible_numbers();

    printf("Bye!\n");
    
    return EXIT_SUCCESS;
}

void init_board(void) {
    int i;
    board = (int **) malloc(sizeof(int *) * size);
    for (i = 0; i < size; i++) {
        board[i] = (int *) malloc(sizeof(int) * size);
        memset(board[i], 0, sizeof(int) * size);
    }
}

void parse_file(const char *path) {
    FILE *fp;
    char c;
    const int BUFF_SIZE = 256;
    char buff[BUFF_SIZE];
    int i, j, k, num;

    fp = fopen(path, "r");

    c = fgetc(fp);
    cube_size = (int) c - '0';
    size = cube_size * cube_size;

    init_board();

    fgets(buff, BUFF_SIZE, fp); // read the first line
    
    i = j = num = 0;
    while (fgets(buff, BUFF_SIZE, fp) != NULL) {
        for (k = 0; k < BUFF_SIZE; k++) {
            c = buff[k];
            
            if (isdigit(c)) {
                num = num * 10 + (int) (c - '0');
            } else {
                if (j >= size) {
                    j = 0;
                }
                
                board[i][j] = num;

                j++;
                num = 0;
                
                if (c == '\n') {
                    i++;
                    break;
                }
            }
        }

        if (i >= size) {
            break;
        }
    }

    fclose(fp);
}

void init_possible_numbers(void) {
    int i, j, k;
    possible_numbers = (int ***) malloc(sizeof(int **) * size);
    for (i = 0; i < size; i++) {
        possible_numbers[i] = (int **) malloc(sizeof(int *) * size);
        for (j = 0; j < size; j++) {
            possible_numbers[i][j] = (int *) malloc(sizeof(int) * size);
            for (k = 0; k < size; k++) {
                possible_numbers[i][j][k] = TRUE;
            }
        }
    }
}

void destroy_board(void) {
    int i;
    for (i = 0; i < size; i++) {
        free(board[i]);
    }

    free(board);
}

void destroy_possible_numbers(void) {
    int i, j;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            free(possible_numbers[i][j]);
        }
        free(possible_numbers[i]);
    }

    free(possible_numbers);
}

int is_num_valid(int num, int x, int y) {
    int i, j;

    const int k = (x / cube_size) * cube_size;
    const int l = (y / cube_size) * cube_size;
    
    for (i = 0; i < size; i++) {
        if (board[x][i] == num) {
            return FALSE;
        } else if (board[i][y] == num) {
            return FALSE;
        }
    }

    for (i = k; i < cube_size + k; i++) {
        if (i == x) {
            continue;
        }
        
        for (j = l; j < cube_size + l; j++) {
            if (j == y) {
                continue;
            }
            
            if (board[i][j] == num) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

void calculate_possible_numbers(int num_index, int x, int y) {
    const int k = (x / cube_size) * cube_size;
    const int l = (y / cube_size) * cube_size;
    
    int i, j;
    
    for (i = 0; i < size; i++) {
        possible_numbers[i][y][num_index] = FALSE;
        possible_numbers[x][i][num_index] = FALSE;
    }

    for (i = k; i < cube_size + k; i++) {
        if (i == x) {
            continue;
        }
        
        for (j = l; j < cube_size + l; j++) {
            if (j == y) {
                continue;
            }
            
            possible_numbers[i][j][num_index] = FALSE;
        }
    }
}

void calculate_possible_numbers_all(void) {
    int i, j;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (board[i][j]) {
                calculate_possible_numbers(board[i][j] - 1, i, j);
            }
        }
    }
}

int solve(int x, int y) {
    if (x >= size) {
        x = 0;
        y++;
        if (y >= size) {
            return TRUE;
        }
    }
    
    if (board[x][y]) {
        return solve(x + 1, y);
    }

    int k;
    for (k = 1; k <= size; k++) {       
        if (possible_numbers[x][y][k - 1]) {
#if DEBUG
            is_num_valid_counter++;
#endif
            if (is_num_valid(k, x, y)) {
                board[x][y] = k;
#if DEBUG
                // print_board();
#endif
                if (solve(x + 1, y)) {
                    return TRUE;
                }
            }
        }
    }

    board[x][y] = 0;
    return FALSE;
}

int solve_iterative(void) {
    Stack stack = {};
    int start_num;
    int solution_found;
    int x, y, k;

    x = y = 0;
    start_num = 1;
    
    while (y < size) {
        while (x < size) {
            if (!board[x][y]) {
                solution_found = FALSE;
                for (k = start_num; k <= size; k++) {
                    if (possible_numbers[x][y][k - 1]) {
                        if (is_num_valid(k, x, y)) {
                            board[x][y] = k;
                            push_stack_cell_num_pos(&stack, x, y, k);
                            solution_found = TRUE;
                            break;
                        }   
                    }
                }

                if (solution_found) {
                    start_num = 1;
                    x++;
                    if (x >= size) {
                        x = 0;
                        y++;
                        if (y >= size) {
                            destroy_stack(&stack);
                            return TRUE;
                        }
                    }
                } else {
                    if (!stack.top) {
                        destroy_stack(&stack);
                        return TRUE;
                    } else {
                        struct cell_num_pos *top = pop_stack(&stack);
                        x = top->x;
                        y = top->y;
                        start_num = top->num + 1;

                        free(top);
                        board[x][y] = 0;
                    }
                }
            } else {
                x++;
                if (x >= size) {
                    x = 0;
                    y++;
                    if (y >= size) {
                        destroy_stack(&stack);
                        return TRUE;
                    }
                }
            }
        }
    }

    destroy_stack(&stack);
    return FALSE;
}

int is_board_valid(void) {
    int i, j, num, result;
    
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            num = board[i][j];
            board[i][j] = 0;
            result = is_num_valid(num, i, j);
            board[i][j] = num;
            
            if (!result) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

void print_possible_numbers(int x, int y) {
    int k;

    printf("[%d, %d]: ", x, y);
    for (k = 0; k < size; k++) {
        printf("%d%s", possible_numbers[x][y][k], ((k < size - 1) ? ", " : ""));
    }
    printf("\n");
}

void print_possible_numbers_all(void) {
    int i, j;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            print_possible_numbers(i, j);
        }
    }
}

void print_board(void) {
    int i, j;

    printf("+");
    for (j = 1; j <= 3 * size; j++) {
        if (j % (cube_size * 2 + cube_size) == 0) {
            printf("+");
        } else {
            printf("-");
        }
    }
    printf("\n");
    
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            printf("%c%2d", (j % cube_size) == 0 ? '|' : ' ', board[i][j]);
        }
        printf("|\n");
    
        if ((i + 1) % cube_size == 0) {
            printf("+");
            for (j = 1; j <= 3 * size; j++) {
                if (j % (cube_size * 2 + cube_size) == 0) {
                    printf("+");
                } else {
                    printf("-");
                }
            }
            printf("\n");
        }
    }
}

void push_stack_cell_num_pos(Stack *stack, int x, int y, int num) {
    struct cell_num_pos *c = (struct cell_num_pos *) malloc(sizeof(struct cell_num_pos));
    c->x = x;
    c->y = y;
    c->num = num;
    c->next = stack->top;
    stack->top = c;
}

struct cell_num_pos * pop_stack(Stack *stack) {
    if (stack->top) {
        struct cell_num_pos *top = stack->top;
        stack->top = stack->top->next;
        return top;
    } else {
        return 0;
    }
}

void destroy_stack(Stack *stack) {
    struct cell_num_pos *next;

    while (stack->top) {
        next = stack->top->next;
        free(stack->top);
        stack->top = next;
    }
}
