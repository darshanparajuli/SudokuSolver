#include "sudoku.h"

#if DEBUG
static struct timespec sleep_time;
static int is_num_valid_counter = 0;
#endif

void init_board(void) {
    int i;
    board = (int **) malloc(sizeof(int *) * size);
    for (i = 0; i < size; i++) {
        board[i] = (int *) malloc(sizeof(int) * size);
        memset(board[i], 0, sizeof(int) * size);
    }
}

int parse_file(const char *path) {
    FILE *fp;
    char c;
    const int BUFF_SIZE = 256;
    char buff[BUFF_SIZE];
    int i, j, k, num;

    fp = fopen(path, "r");

    if (fp == NULL) {
        printf("Cannot open file %s\n", path);
        return FALSE;
    }
    
    c = fgetc(fp);
    cube_size = (int) c - '0';
    size = cube_size * cube_size;

    if (!board) {
        init_board();
    }

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

    return TRUE;
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
    CellNumPos *top;
    int result;

    init_stack(&stack, size * size);
    
    x = y = 0;
    start_num = 1;
    result = FALSE;
    
    while (y < size) {
        while (x < size) {
            if (!board[x][y]) {
                solution_found = FALSE;
                for (k = start_num; k <= size; k++) {
                    if (possible_numbers[x][y][k - 1]) {
#if DEBUG
                        is_num_valid_counter++;
#endif
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
                } else {
                    if (stack.index < 0) {
                        result = FALSE;
                        goto end;
                    } else {
                        top = pop_stack(&stack);
                        x = top->x;
                        y = top->y;
                        start_num = top->num + 1;

                        board[x][y] = 0;
                    }

                    continue;
                }
            }
            
            x++;
            if (x >= size) {
                x = 0;
                y++;
                if (y >= size) {;
                    result = TRUE;
                    goto end;
                }
            }
        }
    }

 end:
    destroy_stack(&stack);
    return result;
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

void init_stack(Stack *stack, int size) {
    stack->data = (CellNumPos*) malloc(sizeof(CellNumPos) * size);
    stack->size = size;
}

void push_stack_cell_num_pos(Stack *stack, int x, int y, int num) {
    if (stack->index < stack->size - 1) {
        CellNumPos *c = &stack->data[++(stack->index)];
        c->x = x;
        c->y = y;
        c->num = num;
    }
}

CellNumPos *pop_stack(Stack *stack) {
    if (stack->index >= 0) {
        return &(stack->data[(stack->index)--]);
    } else {
        return 0;
    }
}

void destroy_stack(Stack *stack) {
    free(stack->data);
}
