
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024

// Global data information
char outputs[500][50];
int output_count = 0;

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

        char digits[LINE_LENGTH];
        char output[LINE_LENGTH];
        if (sscanf(line, "%[a-g ] | %[a-g ]", digits, output) == 2) {
            // Split the data in 'digits' and 'output'
            printf("'%s' --> '%s'\n", digits, output);
            strcpy(outputs[output_count++], output);
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

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argv[1] != NULL) {
        fname = argv[1];
    }

    readData(fname);
    for (int i = 0; i < output_count; i++) {
    }

    // Algorithm
    int known_count = 0;
    for (int i = 0; i < output_count; i++) {
        char *output = outputs[i];

        printf(" --> %s\n", output);

        int digit_len = 1;
        int this_count = 0;
        while (*output != '\000') {
            output++;
            if ((*output == ' ') || (*output == '\000')) {
                switch (digit_len) {
                case 2: // '1'
                case 3: // '7'
                case 4: // '4'
                case 7: // '8'
                    this_count++;
                    break;
                default:
                    break;
                }
                digit_len = 0;
            } else {
                digit_len++;
            }
        }

        known_count += this_count;
    }

    printf("Known digits in output: %d\n", known_count);
    return EXIT_SUCCESS;
}

