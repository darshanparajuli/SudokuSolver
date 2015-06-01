#include "sudoku.h"

void run_solver(int recurr);

int main(int argc, char **argv) {
    const char* path;

    if (argc <= 1) {
        printf("Error: missing path as argument!\n");
        return EXIT_FAILURE;
    }

    path = argv[1];
    
    printf("Initializing...\n");

    if (parse_file(path) == FALSE) {
        return EXIT_FAILURE;
    }

    print_board();

    init_possible_numbers();
    calculate_possible_numbers_all();
    
#if DEBUG
    sleep_time.tv_nsec = 500000;
#endif

    run_solver(0);
#if DEBUG
    parse_file(path);
    run_solver(0);
#endif
    print_board();

    printf("Cleaning up...\n");
    
    destroy_board();
    destroy_possible_numbers();

    printf("Bye!\n");
    
    return EXIT_SUCCESS;
}

void run_solver(int recurr) {
    struct timespec time;
    double beforeTime, afterTime;

#if DEBUG
    is_num_valid_counter = 0;
#endif
    
    printf("Solving %sly...\n", recurr ? "recursive" : "iterative");
    clock_gettime(CLOCK_REALTIME, &time);
    beforeTime = time.tv_sec + (time.tv_nsec / 1000000000.0);

    int result;

    if (recurr) {
        result = solve(0, 0);
    } else {
        result = solve_iterative();
    }
    
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

    printf("Time taken: %.6f seconds\n", (afterTime - beforeTime));
}
