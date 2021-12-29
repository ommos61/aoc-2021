
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
int debug = 1; // 0 = no debug output, 1 = some extra debug output
typedef struct point {
    int x, y, z;
} point;
typedef struct cuboid {
    point start;
    point end;
} cuboid;
#define STATE_OFF 0
#define STATE_ON  1
typedef struct instruction {
    int cmd;
    cuboid step;
} instruction;
instruction instructions[1024];
int instruction_count = 0;

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
            instructions[instruction_count].cmd = (strcmp(cmd, "on") == 0) ? STATE_ON : STATE_OFF;
            instructions[instruction_count].step.start.x = x1;
            instructions[instruction_count].step.start.y = y1;
            instructions[instruction_count].step.start.z = z1;
            instructions[instruction_count].step.end.x = x2;
            instructions[instruction_count].step.end.y = y2;
            instructions[instruction_count].step.end.z = z2;
            instruction_count++;
        } else if (errno != 0) {
            perror("sscanf");
        } else {
            fprintf(stderr, "Unexpected input format '%s'.\n", line);
        }
    }

    printf("Instructions = %d\n", instruction_count);
    fclose(fin);
}

void printInstruction(instruction ex) {
    printf("%3s x=%d->%d, y=%d->%d, z=%d->%d\n",
        (ex.cmd == STATE_ON) ? "ON" : "OFF",
        ex.step.start.x, ex.step.end.x,
        ex.step.start.y, ex.step.end.z,
        ex.step.start.z, ex.step.end.z);
}

int inSpace(point a) {
    int result = 1;
    if ((a.x < -50) || (a.x > 50)) result = 0;
    if ((a.y < -50) || (a.y > 50)) result = 0;
    if ((a.z < -50) || (a.z > 50)) result = 0;
    return result;
}

#define OFFSET 50
void setCuboid(int space[101][101][101], int state, point start, point end) {
    if (inSpace(start) && inSpace(end)) {
        for (int z = start.z + OFFSET; z <= end.z + OFFSET; z++) {
            for (int y = start.y + OFFSET; y <= end.y + OFFSET; y++) {
                for (int x = start.x + OFFSET; x <= end.x + OFFSET; x++) {
                    space[z][y][x] = state;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc != 1) {
        fname = argv[1];
    }

    readData(fname);

    // Count 'on' cubes within -50..50 after all steps are excuted
    int space[101][101][101];
    memset(space, 0, 101 * 101 * 101 * sizeof(int));
    for (int i = 0; i < instruction_count; i++) {
//        printInstruction(instructions[i]);
        int state = instructions[i].cmd;
        setCuboid(space, state, instructions[i].step.start, instructions[i].step.end);
    }

    long count = 0;
    for (int z = 0; z < 101; z++)
        for (int y = 0; y < 101; y++)
            for (int x = 0; x < 101; x++)
                if (space[z][y][x] == STATE_ON)
                    count++;
    printf("Answer part 1: %ld\n", count);
        
    return EXIT_SUCCESS;
}

