
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#define DEBUG

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#define LINE_LENGTH 80

// problem data types
typedef struct point {
    int x;
    int y;
} point;
typedef struct line {
    point start;
    point end;
} line;

#define MAX_LINES 1000
line lines[MAX_LINES];
int lines_count = 0;
int max_x = -1;
int max_y = -1;
int *map = NULL;

void printLine(line *line) {
    printf(" %d, %d -> %d, %d\n", (*line).start.x, (*line).start.y, (*line).end.x, (*line).end.y);
}

void printLines(void) {
#ifdef DEBUG
    for (int i = 0; i < lines_count; i++) {
        printLine(lines + i);
    }
#endif
}
void addLine(line *lines, int x1, int y1, int x2, int y2) {
    assert(lines_count + 1 < MAX_LINES);

    lines[lines_count].start.x = x1;
    lines[lines_count].start.y = y1;
    lines[lines_count].end.x = x2;
    lines[lines_count].end.y = y2;

    max_x = MAX(max_x, x1);
    max_x = MAX(max_x, x2);
    max_y = MAX(max_y, y1);
    max_y = MAX(max_y, y2);
    lines_count++;
}

void readData(char *fname) {
    FILE *fin = fopen(fname, "r");
    if (fin == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char line[LINE_LENGTH];
    while (fgets(line, LINE_LENGTH, fin) != NULL) {
        // Strip the line-end
        if (strlen(line) != 0) line[strlen(line) - 1] = 0;

        int values[4] = {0};
        if (sscanf(line, "%d,%d -> %d, %d", &values[0], &values[1], &values[2], &values[3]) == 4) {
        } else if (errno != 0) {
            perror("sscanf");
        } else {
            fprintf(stderr, "Unexpected input format '%s'.\n", line);
        }

        addLine(lines, values[0], values[1], values[2], values[3]);
    }

    fclose(fin);
}

void printMap(void) {
#ifdef DEBUG
    assert(map != NULL);
    printf("map (%dx%d):\n", max_x + 1, max_y + 1);
    for (int y = 0; y < max_y + 1; y++) {
        for (int x = 0; x < max_x + 1; x++) {
            int val = map[(max_x + 1) * y + x];
            if (val == 0) {
                printf(".");
            } else {
                printf("%1d", val);
            }
        }
        printf("\n");
    }
#endif
}

void createEmptyMap(void) {
    assert(map == NULL);
    assert(max_x >= 0);
    assert(max_y >= 0);

    int size = (max_x + 1) * (max_y + 1) * sizeof(int);
    map = malloc(size);
    memset(map, 0, size);
}

#define horizontal(l) ((l).start.y == (l).end.y)
#define vertical(l) ((l).start.x == (l).end.x)

void incCoord(int x, int y) {
    assert(map != NULL);

    map[(max_x + 1) * y + x] += 1;
}

void drawLines(void) {
    assert(map != NULL);
    // only horizontal and vertical lines are drawn
    printf("drawing lines\n");
    for (int i = 0; i < lines_count; i++) {
        line *l = lines + i;
        printLine(l);

        int x = l->start.x;
        int y = l->start.y;
        int endx = l->end.x;
        int endy = l->end.y;
        int dx = 0;
        int dy = 0;
        if (horizontal(*l)) {
            dx = (endx >= x) ? 1 : -1;
        } else if (vertical(*l)) {
            dy = (endy >= y) ? 1 : -1;
        } else {
            // otherwise diagonal at 45 degrees
            assert(abs(endx - x) == abs(endy - y));
            dx = (endx > x) ? 1 : -1;
            dy = (endy > y) ? 1 : -1;
        }
        // printf("dx = %d, dy = %d\n", dx, dy);
        incCoord(x, y);
        while ((x != endx) || (y != endy)) {
            x += dx;
            y += dy;
            assert((x >= 0) && (y >= 0));
            assert((x <= max_x) && (y <= max_y));
            incCoord(x, y);
        }
        printMap();
    }
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argv[1] != NULL) {
        fname = argv[1];
    }

    readData(fname);
    printf("lines count = %d\n", lines_count);
    printLines();

    createEmptyMap();
    drawLines();
    printMap();

    int overlap = 0;
    for (int i = 0; i < (max_x + 1) * (max_y + 1); i++) {
        if (map[i] > 1) overlap++;
    }
    printf("Overlapping line points: %d\n", overlap);

    return EXIT_SUCCESS;
}
