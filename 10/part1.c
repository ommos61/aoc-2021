
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#define DEBUG
#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024

// Global data information
char *lines[512];
int line_count = 0;

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

        lines[line_count] = malloc(strlen(line) + 1);
        strcpy(lines[line_count], line);

        line_count++;
    }

    printf("lines = %d\n", line_count);
    fclose(fin);
}


char stack[LINE_LENGTH];

int errorValue(char c) {
    if (c == ')') return 3;
    if (c == ']') return 57;
    if (c == '}') return 1197;
    if (c == '>') return 25137;
    return 0;
}

int isClose(char c) {
    return ((c == ')') || (c == ']') || (c == '}') || (c == '>'));
}

int isMatchClose(char open, char close) {
    if (open == '(') return close == ')';
    if (open == '[') return close == ']';
    if (open == '{') return close == '}';
    if (open == '<') return close == '>';
    return 0;
}

int parseLine(char *line) {
    int error = 0;
    int stack_index = 0;
    char *cptr = line;

    while ((error == 0) && (*cptr != '\000')) {
        if (isClose(*cptr)) {
            if (!isMatchClose(stack[stack_index-1], *cptr)) {
                int val = errorValue(*cptr);
                return val;
            } else {
                stack[--stack_index] = '\000';
            }
        } else {
            stack[stack_index++] = *cptr;
            stack[stack_index] = 0;;
        }
        cptr++;
    }

    return error;
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc != 1) {
        fname = argv[1];
    }

    readData(fname);

    // Syntax check
    int error_value = 0;
    for (int i = 0; i < line_count; i++) {
#ifdef DEBUG
        printf("%03d: %s\n", i + 1, lines[i]);
#endif
        error_value += parseLine(lines[i]);
    }

    printf("Error value: %d\n", error_value);

    return EXIT_SUCCESS;
}

