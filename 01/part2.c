
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 80

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc != 1) {
        fname = argv[1];
    }

    char line[LINE_LENGTH];
    FILE *fin = fopen(fname, "r");
    if (fin == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int count = 0;
    int sum;
    int increased = 0;
    int window[3];
    int index = 0;
    while (fgets(line, LINE_LENGTH, fin) != NULL) {
        int depth;
        if (sscanf(line, "%d\n", &depth) == 1) {
            window[index] = depth;
            if (count >= 3) {
                int newsum = window[0] + window[1] + window[2];
//                printf("sum = %d\n", newsum);
                if (newsum > sum) {
                    increased++;
                }
                sum = newsum;
            }
            index = (index + 1) % array_count(window);
        } else if (errno != 0) {
            perror("sscanf");
        } else {
            fprintf(stderr, "Not a number.\n");
        }
        count++;
    }

    printf("lines = %d\n", count);
    printf("increased = %d\n", increased);
    fclose(fin);

    return EXIT_SUCCESS;
}

