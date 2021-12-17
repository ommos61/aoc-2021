
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEBUG
#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024

// Global data information
#define DOTS_MAX 4096
typedef struct dot {
    int x;
    int y;
} dot;
int max_x = 0;
int max_y = 0;
dot dots[DOTS_MAX];
int dot_count = 0;

#define FOLDS_MAX 1024
#define AXIS_X 0
#define AXIS_Y 1
typedef struct fold {
    int axis;
    int offset;
} fold;
fold folds[FOLDS_MAX];
int fold_count;

// Function to read all input data to memory
void readData(char *fname) {
    FILE *fin = fopen(fname, "r");
    if (fin == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char line[LINE_LENGTH];
    int input_section = 0;
    while (fgets(line, LINE_LENGTH, fin) != NULL) {
        // strip line ending
        if (strlen(line) != 0) line[strlen(line) - 1] = 0;

        if (strlen(line) == 0) input_section++;
        if (input_section == 0) {
            int v1 = 0, v2 = 0;
            if (sscanf(line, "%d,%d", &v1, &v2) == 2) {
                // store the data
                if (v1 > max_x) max_x = v1;
                if (v2 > max_y) max_y = v2;
                dots[dot_count].x = v1;
                dots[dot_count].y = v2;
                dot_count++;
            } else if (errno != 0) {
                perror("sscanf");
            } else {
                fprintf(stderr, "Unexpected input format '%s'.\n", line);
            }
        } else {
            char axis = 0;
            int offset = 0;
            if (sscanf(line, "fold along %c=%d", &axis, &offset) == 2) {
                // store the data
                folds[fold_count].axis = (axis == 'x') ? AXIS_X : AXIS_Y;
                folds[fold_count].offset = offset;
                fold_count++;
            } else if (errno != 0) {
                perror("sscanf");
            } else {
                fprintf(stderr, "Unexpected input format '%s'.\n", line);
            }

        }

    }

    printf("Dot count = %d\n", dot_count);
    printf(" Max = (%d, %d)\n", max_x, max_y);
    printf("Fold count = %d\n", fold_count);
    fclose(fin);
}

int existAlready(dot *this, int count, dot *dots) {
    int result = 0;

    for (int i = 0; i < count; i++) {
        if ((this->x == dots[i].x) && (this->y == dots[i].y)) {
            result = 1;
            break;
        }
    }

    return result;
}

void executeFold(int axis, int offset) {
    for (int i = 0; i < dot_count; i++) {
        if (axis == AXIS_Y) {
            if (dots[i].y > offset) {
                dots[i].y = 2 * offset - dots[i].y;
            }
        }
        if (axis == AXIS_X) {
            if (dots[i].x > offset) {
                dots[i].x = 2 * offset - dots[i].x;
            }
        }
    }

    // make unique
    int new_dot_count = 0;
    for (int i = 0; i < dot_count; i++) {
        if (! existAlready(dots + i, new_dot_count, dots)) {
            dots[new_dot_count++] = dots[i];
        }
    }
    dot_count = new_dot_count;

    if (axis == AXIS_Y) max_y = offset - 1; else max_x = offset - 1;
}

void visualizeDots(void) {
    int count = (max_x + 1) * (max_y + 1);
    int *screen = malloc(count * sizeof(int));
    memset(screen, 0, count * sizeof(int));

    for (int i = 0; i < dot_count; i++) {
        screen[dots[i].y * (max_x + 1) + dots[i].x] = 1;
    }

    for (int y = 0; y <= max_y; y++) {
        for (int x = 0; x <= max_x; x++) {
            printf("%c", (screen[y * (max_x + 1) + x] == 1) ? '#' : ' ');
        }
        printf("\n");
    }
    free(screen);
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc != 1) {
        fname = argv[1];
    }

    readData(fname);

    // Execute the folds
    for (int i = 0; i < fold_count; i++) {
        executeFold(folds[i].axis, folds[i].offset);
        printf("After fold %d: %d dots\n", i + 1, dot_count);
    }

    visualizeDots();

    return EXIT_SUCCESS;
}

