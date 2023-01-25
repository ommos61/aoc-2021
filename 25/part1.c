
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

// Global data information
int debug = 0; // 0 = no debug output, 1 = some extra debug output

int width = 0, height = 0;
#define MAX_LINES 200
char *field[MAX_LINES] = { NULL };

// Function to read all input data to memory
void readData(char *fname) {
    FILE *fin = fopen(fname, "r");
    if (fin == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int line_count = 0;
    char line[LINE_LENGTH];
    while (fgets(line, LINE_LENGTH, fin) != NULL) {
        // strip line ending
        if (strlen(line) != 0) line[strlen(line) - 1] = 0;

        if (strlen(line) != 0) {
            // store the data
            if (width == 0) width = strlen(line);
            assert((strlen(line) == (unsigned)width) && "Widths of the lines must be the same");
            field[height] = malloc(strlen(line) + 1);
            strcpy(field[height], line);
            height += 1;
            assert(height < MAX_LINES);
        } else if (errno != 0) {
            perror("sscanf");
        } else {
            fprintf(stderr, "Unexpected input format '%s'.\n", line);
        }

        line_count++;
    }

    printf("lines = %d\n", line_count);
    fclose(fin);
}

void print_field(void) {
    printf("---------- Field ---------\n");
    for (int h = 0; h < height; h++) {
        printf("%s\n", field[h]);
    }
    printf("\n");
}

int step_field(void) {
    int moved = 0;
    char do_move[height][width];

    // move right
    memset(do_move, 0, width * height);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if ((field[y][x] == '>') && (field[y][(x + 1) % width] == '.')) {
                do_move[y][x] = 1;
            }
        }
        for (int x = 0; x < width; x++) {
            if (do_move[y][x]) {
                field[y][(x + 1) % width] = '>';
                field[y][x] = '.';
                do_move[y][x] = 0;
                moved += 1;
            }
        }
    }
    //print_field();

    // move down
    memset(do_move, 0, width * height);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            if ((field[y][x] == 'v') && (field[(y + 1) % height][x] == '.')) {
                do_move[y][x] = 1;
            }
        }
        for (int y = 0; y < height; y++) {
            if (do_move[y][x]) {
                field[(y + 1) % height][x] = 'v';
                field[y][x] = '.';
                do_move[y][x] = 0;
                moved += 1;
            }
        }
    }
    //print_field();

    return moved;
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc != 1) {
        fname = argv[1];
    }

    readData(fname);
    printf("Field is %dx%d.\n", width, height);
    if (debug) print_field();

    // implement algorithm
    int step = 0, count;
    do {
        step += 1;
        count = step_field();
        if (debug) printf("Step %d: %d moves.\n", step, count);
    } while (count != 0);
    printf("No movement detected on step %d.\n", step);
    if (debug) print_field();

    return EXIT_SUCCESS;
}

