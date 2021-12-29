
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

// Global data information
int debug = 1; // 0 = no debug output, 1 = some extra debug output
typedef struct point {
    int x, y, z;
} point;
#define STATE_OFF 0
#define STATE_ON  1
typedef struct cuboid {
    int state;
    point start;
    point end;
} cuboid;
#define CUBOID_MAX 512
cuboid cuboids[CUBOID_MAX];
int cuboid_count = 0;

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

        char cmd[5];
        int x1, x2, y1, y2, z1, z2;
        if (sscanf(line, "%[a-z] x=%d..%d,y=%d..%d,z=%d..%d", cmd, &x1, &x2, &y1, &y2, &z1, &z2) == 7) {
            assert(x1 < x2); assert(y1 < y2); assert(z1 < z2);
            cuboids[cuboid_count].state = (strcmp(cmd, "on") == 0) ? STATE_ON : STATE_OFF;
            cuboids[cuboid_count].start.x = x1;
            cuboids[cuboid_count].start.y = y1;
            cuboids[cuboid_count].start.z = z1;
            cuboids[cuboid_count].end.x = x2 + 1;
            cuboids[cuboid_count].end.y = y2 + 1;
            cuboids[cuboid_count].end.z = z2 + 1;
            cuboid_count++;
        } else if (errno != 0) {
            perror("sscanf");
        } else {
            fprintf(stderr, "Unexpected input format '%s'.\n", line);
        }
    }

    printf("Cuboids = %d\n", cuboid_count);
    fclose(fin);
}

void printCuboid(cuboid *this, char *prefix) {
    if (prefix != NULL) printf("%s", prefix);
    printf("(x(%d->%d), y(%d->%d), z(%d->%d))",
        this->start.x, this->end.x,
        this->start.y, this->end.y,
        this->start.z, this->end.z);
    if (prefix != NULL) printf("\n");
}

void printStep(cuboid this) {
    printCuboid(&this, (this.state == STATE_ON) ? "ON" : "OFF");
}

int compareCoordinates(const void *p1, const void *p2) {
    return *(long *)p1 - *(long *)p2;
}

long xaxis[2 * CUBOID_MAX];
long yaxis[2 * CUBOID_MAX];
long zaxis[2 * CUBOID_MAX];
char states[2 * CUBOID_MAX][2 * CUBOID_MAX][2 * CUBOID_MAX];

int bsearch_i(const void *key, const void *base,
        size_t nmemb, size_t size, int (*compar)(const void *, const void *)) {
    void *result = bsearch(key, base, nmemb, size, compar);

    assert(sizeof(char) == 1);
    return ((char *)result - (char *)base) / size;
}

void handleCuboid(cuboid *this) {
    int x_start = bsearch_i(&this->start.x, xaxis, 2 * cuboid_count, sizeof(long), compareCoordinates);
    int x_end   = bsearch_i(&this->end.x, xaxis, 2 * cuboid_count, sizeof(long), compareCoordinates);
    int y_start = bsearch_i(&this->start.y, yaxis, 2 * cuboid_count, sizeof(long), compareCoordinates);
    int y_end   = bsearch_i(&this->end.y, yaxis, 2 * cuboid_count, sizeof(long), compareCoordinates);
    int z_start = bsearch_i(&this->start.z, zaxis, 2 * cuboid_count, sizeof(long), compareCoordinates);
    int z_end   = bsearch_i(&this->end.z, zaxis, 2 * cuboid_count, sizeof(long), compareCoordinates);

    for (int x = x_start; x < x_end; x++) {
        for (int y = y_start; y < y_end; y++) {
            for (int z = z_start; z < z_end; z++) {
                states[x][y][z] = this->state;
            }
        }
    }
}

long countOn(void) {
    long count = 0;
    for (int x = 0; x < 2 * cuboid_count - 1; x++) {
        for (int y = 0; y < 2 * cuboid_count - 1; y++) {
            for (int z = 0; z < 2 * cuboid_count - 1; z++) {
                if (states[x][y][z] == STATE_ON) {
                    count += (xaxis[x+1] - xaxis[x]) * (yaxis[y+1] - yaxis[y]) * (zaxis[z+1] - zaxis[z]);
                }
            }
        }
    }
    return count;
}
        
int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc != 1) {
        fname = argv[1];
    }

    readData(fname);
    //cuboid_count = 1;

    // Count 'on' cubes
    for (int i = 0; i < cuboid_count; i++) {
        xaxis[2 * i    ] = cuboids[i].start.x;
        xaxis[2 * i + 1] = cuboids[i].end.x;
        yaxis[2 * i    ] = cuboids[i].start.y;
        yaxis[2 * i + 1] = cuboids[i].end.y;
        zaxis[2 * i    ] = cuboids[i].start.z;
        zaxis[2 * i + 1] = cuboids[i].end.z;
    }
    qsort(xaxis, 2 * cuboid_count, sizeof(long), compareCoordinates);
    qsort(yaxis, 2 * cuboid_count, sizeof(long), compareCoordinates);
    qsort(zaxis, 2 * cuboid_count, sizeof(long), compareCoordinates);

    printf("sizeof(states) = %ld\n", sizeof(states));
    memset(states, 0, sizeof(states));
    long count = countOn();
    printf("ON count before filling in the cuboids: %ld\n", count);
#define LIMIT 50
    int i;
    for (i = 0; i < cuboid_count && (cuboids[i].start.x >= -LIMIT) && (cuboids[i].end.x <= LIMIT); i++) {
//        printStep(cuboids[i]);
        handleCuboid(&cuboids[i]);
    }
    count = countOn();
    printf("Answer part 1: %ld\n", count);
    for (; i < cuboid_count; i++) {
        handleCuboid(&cuboids[i]);
    }

    // count the 'on' cubes
    count = countOn();
    printf("Answer part 2: %ld\n", count);
    printf("LONG_MAX:      %ld\n", LONG_MAX);        

    exit(EXIT_SUCCESS);
}


