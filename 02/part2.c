
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
    int position = 0;
    int depth = 0;
    int aim = 0;
    while (fgets(line, LINE_LENGTH, fin) != NULL) {
        if (strlen(line) != 0) line[strlen(line) - 1] = 0;
        char command[LINE_LENGTH];
        int value = 0;
        if (sscanf(line, "%s %d", command, &value) == 2) {
            if (strcmp(command, "forward") == 0) {
                position += value;
                depth += aim * value;
            } else if (strcmp(command, "down") == 0) {
                aim += value;
            } else if (strcmp(command, "up") == 0) {
                aim -= value;
            } else {
                fprintf(stderr, "Unknown command '%s'\n", command);
            }
        } else if (errno != 0) {
            perror("sscanf");
        } else {
            fprintf(stderr, "Unexpected input format '%s'.\n", line);
        }
        count++;
    }

    printf("lines = %d\n", count);
    printf("position = %d\n", position);
    printf("depth = %d\n", depth);
    printf("multiplication = %d\n", position * depth);
    fclose(fin);

    return EXIT_SUCCESS;
}

