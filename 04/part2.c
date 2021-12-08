
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#define DEBUG

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 4096

#define BOARD_WIDTH 5
#define BOARD_SIZE (BOARD_WIDTH * BOARD_WIDTH)
typedef struct board {
    int values[BOARD_SIZE];
    int checks[BOARD_SIZE];
    int finished;
} board;

int draw_count = 0;
int *draws = NULL;
int board_count = 0;
board *boards = NULL;

void printBoard(board *my_board) {
#ifdef DEBUG
    for (int i = 0; i < BOARD_SIZE; i++) {
        if ((i % BOARD_WIDTH) == 0) printf("\n");
        printf("%-2d%s ", my_board->values[i], my_board->checks[i] ? "*" : " ");
    }
    printf("\n");
#endif
}

void readData(char *fname) {
    FILE *fin = fopen(fname, "r");
    if (fin == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Read the file data
    char line[LINE_LENGTH + 1];

    // Read the draw numbers (comma separated one a single line)
    fgets(line, LINE_LENGTH, fin);

    int index = 0;
    while (index < strlen(line)) {
        if (line[index] == ',') draw_count++;
        index++;
    }
    draw_count++;

    draws = malloc(draw_count * sizeof(int));
    char *cptr = line;
    int draw_index = 0;
    while (cptr < line + strlen(line)) {
        int cnt = sscanf(cptr, "%d", draws + draw_index);
        if (cnt == EOF) {
            perror("sscanf");
            exit(EXIT_FAILURE);
        }

        draw_index++;
        while (*cptr && (*cptr != ',')) cptr++;
        if (*cptr == ',') cptr++;
    }

    // Read the boards
    size_t filepos = ftell(fin);

    while (fgets(line, LINE_LENGTH, fin)) {
        for (int i = 0; i < BOARD_WIDTH; i++) {
            fgets(line, LINE_LENGTH, fin);
        }
        board_count++;
    }
    
    boards = malloc(board_count * sizeof(board));
    memset(boards, 0, board_count * sizeof(board));
    fseek(fin, filepos, SEEK_SET);
    int board_index = 0;
    while (fgets(line, LINE_LENGTH, fin)) {
        for (int i = 0; i < BOARD_WIDTH; i++) {
            fgets(line, LINE_LENGTH, fin);
            int cnt = sscanf(line, "%d %d %d %d %d",
                        &boards[board_index].values[5 * i],
                        &boards[board_index].values[5 * i + 1],
                        &boards[board_index].values[5 * i + 2],
                        &boards[board_index].values[5 * i + 3],
                        &boards[board_index].values[5 * i + 4]);
            if (cnt == EOF) {
                perror("sscanf");
                exit(EXIT_FAILURE);
            }
        }
        board_index++;
    }

    fclose(fin);
}

int checkBoard(board *my_board, int draw_value) {
    int result = 0;

    // Check off the board location when the number is on the board
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (my_board->values[i] == draw_value) {
            my_board->checks[i] = 1;
        }
    }

    // Check if a row is complete
    for (int i = 0; i < BOARD_WIDTH; i++) {
        int cnt = 0;
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (my_board->checks[5 * i + j]) {
                cnt++;
            }
        }
        result |= (cnt == 5);
    }

    // Check if a column is complete
    for (int i = 0; i < BOARD_WIDTH; i++) {
        int cnt = 0;
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (my_board->checks[5 * j + i]) {
                cnt++;
            }
        }
        result |= (cnt == 5);
    }

    my_board->finished = result;

    if (result) {
        printBoard(my_board);
    }
    return result;
}

int checkBoards(int draw_value) {
    int result = -1;

    for (int i = 0; i < board_count; i++) {
        if (!boards[i].finished && checkBoard(boards + i, draw_value)) {
            result = i;
        }
    }

    return result;
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // When another input file is specified
    if (argv[1] != NULL) {
        fname = argv[1];
    }

    readData(fname);
    printf("Draw count = %d\n", draw_count);
    printf("Board count = %d\n", board_count);
    for (int i = 0; i < board_count; i++) {
        printBoard(boards + i);
    }

    int still_drawing = 1;
    int draw_index = 0;
    board *winning_board = NULL;
    while (still_drawing) {
        printf("-----> Current draw: %d\n", draws[draw_index]);

        // Check the boards against the draw
        int winning_index = checkBoards(draws[draw_index]);
        if (winning_index != -1) {
            winning_board = boards + winning_index;
        }

        // Check how many boards are left
        int boards_left = board_count;
        for (int i = 0; i < board_count; i++) {
            if (boards[i].finished) boards_left -= 1;
        }
        printf("Boards left: %d\n", boards_left);

        // Update
        draw_index++;
        still_drawing = (draw_index < draw_count) && (boards_left != 0);
    }

    // Make sure we have a winning board
    assert(winning_board != NULL);

    // Add the unmarked items on the winning board
    int sum = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (! winning_board->checks[i]) {
            sum += winning_board->values[i];
        }
    }
    printf("Winning board sum is %d\n", sum);
    int last_draw = draws[draw_index - 1];
    printf("Last draw is %d\n", last_draw);

    printf("Answer is %d\n", last_draw * sum);

    return EXIT_SUCCESS;
}

