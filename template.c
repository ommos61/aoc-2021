
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 80

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argv[1] != NULL) {
        fname = argv[1];
    }

    char line[LINE_LENGTH];
    FILE *fin = fopen(fname, "r");
    if (fin == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int count = 0;
    while (fgets(line, LINE_LENGTH, fin) != NULL) {
        if (strlen(line) != 0) line[strlen(line) - 1] = 0;
        int value = 0;
        if (sscanf(line, "%d", &value) == 1) {
        } else if (errno != 0) {
            perror("sscanf");
        } else {
            fprintf(stderr, "Unexpected input format '%s'.\n", line);
        }
        count++;
    }

    printf("lines = %d\n", count);
    fclose(fin);

    return EXIT_SUCCESS;
}

