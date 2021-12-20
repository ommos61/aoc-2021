
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

// TODO: Global data information
int debug = 1; // 0 = no debug output, 1 = some extra debug output
typedef struct point {
    int x;
    int y;
} point;
point target[2];

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

        int x1, x2, y1, y2;
        printf("input line: %s\n", line);
        if (sscanf(line, "target area: x=%d..%d, y=%d..%d", &x1, &x2, &y1, &y2) == 4) {
            // store the read values
            target[0].x = x1; target[0].y = y1;
            target[1].x = x2; target[1].y = y2;
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

int isTarget(int x, int y) {
    int result = 1;

    if ((x < target[0].x) || (x > target[1].x)) result = 0;
    if ((y < target[0].y) || (y > target[1].y)) result = 0;

    return result;
}

int isOvershoot(int x, int y) {
    return ((x > target[1].x) || (y < target[0].y));
}

int performStep(point *pos, point *vel) {
    pos->x += vel->x;
    pos->y += vel->y;
    if (vel->x > 0) vel->x--;
    vel->y--;

    return isTarget(pos->x, pos->y) || isOvershoot(pos->x, pos->y);
}


int simulate(point velocity, int *max_height) {
    point position = { 0, 0};
    int finished = 0;
    while (! finished) {
        finished = performStep(&position, &velocity);
        *max_height = MAX(*max_height, position.y);
        if (debug) printf("New position: (%d, %d)\n", position.x, position.y);
    }
    if (isTarget(position.x, position.y)) {
        if (debug) printf("----> Final position: (%d, %d)\n", position.x, position.y);
        if (debug) printf("      %s\n", isTarget(position.x, position.y) ? "Hit" : "Miss");
    }

    return isTarget(position.x, position.y);
}

void testVelocity(int x, int y) {
    point velocity = { x, y};

    int height = 0;
    int hit = simulate(velocity, &height);
    printf("Test velocity (%d,%d) = %s\n", velocity.x, velocity.y, (hit) ? "Hit" : "Miss");
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc != 1) {
        fname = argv[1];
    }

    readData(fname);
    printf("Area: (%d, %d) -> (%d, %d)\n", target[0].x, target[0].y, target[1].x, target[1].y);

    // Implement algorithm
    // - vel.x can be calculated, because with the constant drag and 0 end velocity,
    //   it is an arithmetic sequence:
    //      n + (n-1) + (n-2) + ... + 1 + 0
    //   or: 1 + 2 + 3 ... + n  -> n * (1 + n) / 2
    int vel_x = 0;
    int pos_x = 0;
    while (pos_x < target[0].x) {
        vel_x += 1;
        pos_x += vel_x;
    }
    int min_vel_x = vel_x;
    printf("Minimum velocity.x: %d\n", min_vel_x);
    while (pos_x <= target[1].x) {
        vel_x += 1;
        pos_x += vel_x;
    }
    int max_vel_x = vel_x - 1;
    printf("Maximum velocity.x: %d\n", max_vel_x);

    debug = 0;
    int max_vel_y = 500;
    int hit_count = 0;
    for (int vel_x = 0; vel_x <= target[1].x; vel_x++) {
        for (int vel_y = MIN(target[0].y, target[1].y); vel_y <= max_vel_y; vel_y++) {
            point velocity = { vel_x, vel_y};
            int height = 0;
            if (simulate(velocity, &height)) {
                if (debug) printf("Velocity: %d,%d\n", velocity.x, velocity.y);
                hit_count++;
            }
        }
    }
    printf("Hit count:  %d\n", hit_count);
    
    debug = 0;
    if (debug) testVelocity(20, -6);

    return EXIT_SUCCESS;
}

