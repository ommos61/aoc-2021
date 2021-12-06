
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#define LINE_LENGTH 80

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

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
    int previous = -1;
    int increased = 0;
    while (fgets(line, LINE_LENGTH, fin) != NULL) {
        int depth;
        //printf("line = '%s'.\n", line);
        if (sscanf(line, "%d\n", &depth) == 1) {
            if ((previous != -1) && (depth > previous)) {
                increased++;
            }
            previous = depth;
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

