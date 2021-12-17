
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024

// Global data information
#define ELEMENT_COUNT 10
int map[ELEMENT_COUNT][ELEMENT_COUNT];

// Function to read all input data to memory
void readData(char *fname) {
    FILE *fin = fopen(fname, "r");
    if (fin == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int line_index = 0;
    char line[LINE_LENGTH];
    while (fgets(line, LINE_LENGTH, fin) != NULL) {
        // strip line ending
        if (strlen(line) != 0) line[strlen(line) - 1] = 0;

        for (int i = 0; i < ELEMENT_COUNT; i++) {
            map[line_index][i] = line[i] - '0';
        }

        line_index++;
    }

    fclose(fin);
}

int flashed[ELEMENT_COUNT][ELEMENT_COUNT];
int flash_count = 0;

void printMap(void) {
    for (int y = 0; y < ELEMENT_COUNT; y++) {
        for (int x = 0; x < ELEMENT_COUNT; x++) {
            if (map[y][x] == 0) {
                // bold
                printf("\033[1m0\033[0m");
            } else {
                printf("%d", map[y][x]);
            }
        }
        printf("\n");
    }
    printf("\n");
}

struct {
    int dx; int dy;
} directions[] = {
    { 0, 1 },
    { 0, -1 },
    { 1, 0 },
    { -1, 0 },
    { 1, 1 },
    { 1, -1 },
    { -1, 1 },
    { -1, -1 }
};

void flash(int x, int y);

void touch(int x, int y) {
    if ((x < 0) || (x >= ELEMENT_COUNT)) return;
    if ((y < 0) || (y >= ELEMENT_COUNT)) return;

    map[y][x] += 1;
    if (map[y][x] > 9) flash(x, y);
}

void flash(int x, int y) {
    if (! flashed[y][x]) {
        flashed[y][x] = 1;
        flash_count++;

        // update neighbors
        for (unsigned int i = 0; i < array_count(directions); i++) {
            touch(x + directions[i].dx, y + directions[i].dy);
        }
    }
}

void performStep(int step) {
    // clear the flashes for this step
    flash_count = 0;
    memset(flashed, 0, sizeof(int) * ELEMENT_COUNT * ELEMENT_COUNT);

    for (int y = 0; y < ELEMENT_COUNT; y++) {
        for (int x = 0; x < ELEMENT_COUNT; x++) {
            map[y][x] += 1;
        }
    }
    for (int y = 0; y < ELEMENT_COUNT; y++) {
        for (int x = 0; x < ELEMENT_COUNT; x++) {
            if (map[y][x] > 9) flash(x, y);
        }
    }
    for (int y = 0; y < ELEMENT_COUNT; y++) {
        for (int x = 0; x < ELEMENT_COUNT; x++) {
            if (map[y][x] > 9) map[y][x] = 0;;
        }
    }
    if (flash_count == ELEMENT_COUNT * ELEMENT_COUNT) {
        printf("All octopuses flashed after step %d\n", step);
    }
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc != 1) {
        fname = argv[1];
    }

    readData(fname);
    printf("Before any steps:\n");
    printMap();

    // Progress steps
    int step = 0;
    while (step < 500) {
        step++;
        performStep(step);
//        if (step <= 2) {
//            printf("After step %d:\n", step);
//            printMap();
//        }
    }

    return EXIT_SUCCESS;
}

