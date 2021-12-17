
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

//#define DEBUG
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
        assert((unsigned)map_width == strlen(line));

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

typedef struct basin {
    int x;
    int y;
    int size;
} basin;
basin largest_basins[3];
int basin_min = 0;
char *basin_checks = NULL;

#define h(x,y) (map[y][x])

void printMap(void) {
#ifdef DEBUG
    for (int y = 0; y < map_lines; y++) {
        for (int x = 0; x < map_width; x++) {
            printf("%d", map[y][x]);
        }
        printf("   ");
        for (int x = 0; x < map_width; x++) {
            printf("%c", basin_checks[y * map_width + x] ? 'X' : '.');
        }
        printf("\n");
    }
#endif
}

int basinCheck(int in_size, int x, int y) {
    int size = in_size;

    if ((x < 0) || (x >= map_width)) return size;
    if ((y < 0) || (y >= map_lines)) return size;

    if (map[y][x] == 9) return size;

    int offset = y * map_width + x;
    if (basin_checks[offset]) return size;

    basin_checks[offset] = 1;
    size++;
    int h = h(x, y);
    size = ((x < map_width-1) && (h(x+1,y) > h)) ? basinCheck(size, x + 1, y) : size;
    size = ((x > 0) && (h(x-1,y) > h)) ? basinCheck(size, x - 1, y) : size;
    size = ((y < map_lines-1) &&(h(x,y+1) > h)) ? basinCheck(size, x, y + 1) : size;
    size = ((y > 0) && (h(x,y-1) > h)) ? basinCheck(size, x, y - 1) : size;

    return size;
}

int basinSize(int x, int y) {
    if (basin_checks == NULL) {
        basin_checks = malloc(map_lines * map_width * sizeof(char));
    }
    memset(basin_checks, 0, map_lines * map_width);

    // recursively get the size of the basin
    int size = basinCheck(0, x, y);
    return size;
}

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MIN3(a,b,c) (MIN((a), MIN((b), (c))))

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc != 1) {
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
    for (int y = 0; y < map_lines; y++) {
        for (int x = 0; x < map_width; x++) {
            if (local_min(x, y)) {
                min_count++;
                int basin_size = basinSize(x, y);
//                printf("Basin size: %d\n", basin_size);
                printMap();
                if (basin_size > basin_min) {
                    int current_min = largest_basins[0].size;
                    int current_min_index = 0;
                    if (current_min > largest_basins[1].size) {
                        current_min = largest_basins[1].size;
                        current_min_index = 1;
                    }
                    if (current_min > largest_basins[2].size) {
                        current_min = largest_basins[2].size;
                        current_min_index = 2;
                    }
//                    printf("Setting new lowest max at index %d\n", current_min_index);
                    largest_basins[current_min_index].size = basin_size;
                    largest_basins[current_min_index].x = x;
                    largest_basins[current_min_index].y = y;
                    basin_min = MIN3(largest_basins[0].size, largest_basins[1].size, largest_basins[2].size);
                }
            }
        }
    }

    printf("Minima count: %d\n", min_count);
    int answer = largest_basins[0].size * largest_basins[1].size * largest_basins[2].size;
    printf("Basin answer: %d\n", answer);

    return EXIT_SUCCESS;
}

