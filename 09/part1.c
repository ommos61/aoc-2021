
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024

// TODO: Global data information
#define MAX_LINES 1024
char *map[MAX_LINES];
int map_width = 0;
int map_lines = 0;

// Function to read all input data to memory
void readData(char *fname) {
    FILE *fin = fopen(fname, "r");
    if (fin == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char line[LINE_LENGTH];
    while (fgets(line, LINE_LENGTH, fin) != NULL) {
        // strip line ending
        if (strlen(line) != 0) line[strlen(line) - 1] = 0;

        if (map_width == 0) map_width = strlen(line);
        assert(map_width == strlen(line));

        map[map_lines] = malloc(strlen(line) + 1);
        strcpy(map[map_lines], line);

        map_lines++;
    }

    printf("width = %d\n", map_width);
    printf("lines = %d\n", map_lines);
    fclose(fin);
}

int local_min(int x, int y) {
    int high_count = 0;

    if (y == 0) {
        high_count++;
    } else {
        high_count += (map[y][x] < map[y-1][x]) ? 1 : 0;
    }
    if (y == map_lines - 1) {
        high_count++;
    } else {
        high_count += (map[y][x] < map[y+1][x]) ? 1 : 0;
    }

    if (x == 0) {
        high_count++;
    } else {
        high_count += (map[y][x] < map[y][x-1]) ? 1 : 0;
    }
    if (x == map_width - 1) {
        high_count++;
    } else {
        high_count += (map[y][x] < map[y][x+1]) ? 1 : 0;
    }

    return (high_count == 4);
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argv[1] != NULL) {
        fname = argv[1];
    }

    readData(fname);

    // Convert the ascii characters to numbers
    for (int y = 0; y < map_lines; y++) {
        for (int x = 0; x < map_width; x++) {
            map[y][x] = map[y][x] - '0';
        }
    }

    // Calculate map minima
    int min_count = 0;
    int risk_sum = 0;
    for (int y = 0; y < map_lines; y++) {
        for (int x = 0; x < map_width; x++) {
            if (local_min(x, y)) {
                min_count++;
                risk_sum += map[y][x] + 1;
            }
        }
    }

    printf("Minima count: %d\n", min_count);
    printf("Risk sum: %d\n", risk_sum);

    return EXIT_SUCCESS;
}

