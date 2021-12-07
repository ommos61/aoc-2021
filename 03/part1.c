
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 80

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc > 1) {
        fname = argv[1];
    }

    char line[LINE_LENGTH];
    FILE *fin = fopen(fname, "r");
    if (fin == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int count = 0;
    int bitcount = 0;
    int *counts = NULL;
    while (fgets(line, LINE_LENGTH, fin) != NULL) {
        if (strlen(line) != 0) line[strlen(line) - 1] = 0;

        // create the counts array
        if (counts == NULL) {
            bitcount = strlen(line);
            counts = malloc(bitcount * sizeof(int));
            for (int i = 0; i < bitcount; i++) {
                counts[i] = 0;
            }
        }

        for (int i = 0; i < bitcount; i++) {
            if (line[i] == '1') {
                counts[i] += 1;
            }
        }

        count++;
    }

    int gamma = 0;
    int epsilon = 0;
    for (int i = 0; i < bitcount; i++) {
        gamma = 2 * gamma + (counts[i] >= (count / 2) ? 1 : 0);
        epsilon = 2 * epsilon + (counts[i] <= (count / 2) ? 1 : 0);
        if (counts[i] == (count / 2)) {
            fprintf(stderr, "Warning: the bitcounts for 0 and 1 are the same.\n");
        }
    }

    printf("lines = %d\n", count);
    printf("gamma rate = %d\n", gamma);
    printf("epsilon rate = %d\n", epsilon);
    printf("answer = %d\n", gamma * epsilon);
    fclose(fin);

    return EXIT_SUCCESS;
}

