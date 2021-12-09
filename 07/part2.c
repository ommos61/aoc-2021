
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 4096
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

// Global data information
#define MAX_POSITIONS 4096
int positions[MAX_POSITIONS];
int position_count = 0;

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

        char *cptr = line;
        while (cptr < line + strlen(line)) {
            int value = 0;
            int nread = sscanf(cptr, "%d", &value);
            if ((nread == EOF) || (nread != 1)) {
                perror("sscanf");
                exit(EXIT_FAILURE);
            }
            
            positions[position_count] = value;
            position_count++;

            while (*cptr && (*cptr != ',')) cptr++;
            if (*cptr == ',') cptr++;
        }
    }

    printf("positions = %d\n", position_count);
    fclose(fin);
}

int position_min = INT_MAX;
int position_max = -1;

int fuelCost(int distance) {
    int result = 0;
    int step_cost = 1;

    for (int i = 0; i < distance; i++) {
        result += step_cost;
        step_cost++;
    }

    return result;
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argv[1] != NULL) {
        fname = argv[1];
    }

    readData(fname);

    // determine the min and max
    for (int i = 0; i < position_count; i++) {
        if (positions[i] < position_min) position_min = positions[i];
        if (positions[i] > position_max) position_max = positions[i];
    }
    printf("Minimum position: %d\n", position_min);
    printf("Maximum position: %d\n", position_max);

    int fuel_min = INT_MAX;
    int fuel_min_target = -1;
    for (int pos = position_min; pos <= position_max; pos++) {
        int fuel = 0;
        for (int i = 0; i < position_count; i++) {
            fuel += fuelCost(abs(pos - positions[i]));
        }

        if (fuel < fuel_min) {
            fuel_min_target = pos;
            fuel_min = fuel;
        }
    }

    assert(fuel_min != INT_MAX);
    assert(fuel_min_target != -1);

    printf("Target position: %d\n", fuel_min_target);
    printf("Fuel cost: %d\n", fuel_min);

    return EXIT_SUCCESS;
}

