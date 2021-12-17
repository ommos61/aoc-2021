
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAx(a,b) (((a) > (b)) ? (a) : (b))

// Global data information
#define DIM_MAX 128
int risks[DIM_MAX][DIM_MAX];
int dim = 0;

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

        dim = strlen(line);
        for (int i = 0; i < dim; i++) {
            risks[line_count][i] = line[i] - '0';
        }

        line_count++;
    }

    printf("lines = %d\n", line_count);
    fclose(fin);
}

void printRisks(int *map, int dim) {
    for (int y = 0; y < dim; y++) {
        for (int x = 0; x < dim; x++) {
            printf(" %4d", map[y * DIM_MAX + x]);
        }
        printf("\n");
    }
}

// Implementation of 'Dijkstra's shortest path' algorithm
int neighbors[][2] = {
    { 0, 1},
    { 1, 0},
    { 0, -1},
    { -1, 0}
};
typedef struct cell {
    int x;
    int y;
} cell;
#define isSafe(x,b) (((x) >= 0) && ((x) < dim) && ((y) >= 0) && ((y) < dim))

#define Q_MAX   100000
cell q[Q_MAX];
int q_size = 0;
int q_begin = 0;

void q_push(int x, int y) {
    int index;
    if (q_size < Q_MAX) {
        index = (q_begin + q_size) % Q_MAX;
        q[index].x = x;
        q[index].y = y;
        q_size++;
    } else {
        printf("Error: Q Overflow!!!\n");
    }
}

void q_pop(int *x, int *y) {
    if (q_size > 0) {
        *x = q[q_begin].x;
        *y = q[q_begin].y;
        q_begin = (q_begin + 1) % Q_MAX;
        q_size--;
    } else {
        printf("Error: Q Underflow!!!\n");
    }
}

int getRisk(int x, int y) {
    int result = risks[y % dim][x % dim] + y / dim + x / dim;

    result = (result - 1) % 9 + 1;
    
    return result;
}

int minRisk3(int x, int y) {
    int dp[5 * DIM_MAX][ 5 * DIM_MAX];

    for (int i = 0; i < 5 * DIM_MAX; i++) {
        for (int j = 0; j < 5 * DIM_MAX; j++) {
            dp[i][j] = INT_MAX;
        }
    }

    dp[0][0] = 0; // getRisk(0, 0);
    q_push(0, 0);

    while (q_size > 0) {
        int cx, cy;
        q_pop(&cx, &cy);
        for (unsigned int n = 0; n < array_count(neighbors); n++) {
            int new_x = cx + neighbors[n][0];
            int new_y = cy + neighbors[n][1];
            if ((new_x >= 0) && (new_x < 5 * dim)) {
                if ((new_y >= 0) && (new_y < 5 * dim)) {
                    if (dp[cy][cx] + getRisk(new_x, new_y) < dp[new_y][new_x]) {
                        dp[new_y][new_x] = dp[cy][cx] + getRisk(new_x, new_y);
                        q_push(new_x, new_y);
                    }
                }
            }
        }
    }

    return dp[y][x];
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc != 1) {
        fname = argv[1];
    }

    readData(fname);
    printf("Dimension: %dx%d\n", dim, dim);
    printf("Individual risks:\n");
    printRisks(risks[0], MIN(dim, 20));
    printf("---------\n");

    // determine cheapest route
    int total = minRisk3(5 * dim - 1, 5 * dim - 1);

    printf("Total risk is: %d\n", total);

    return EXIT_SUCCESS;
}


