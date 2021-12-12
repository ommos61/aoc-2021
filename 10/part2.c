
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

int closeScore(char c) {
    if (c == '(') return 1;
    if (c == '[') return 2;
    if (c == '{') return 3;
    if (c == '<') return 4;
    return 0;
}

int compare_long(const void *a, const void *b) {
    long diff = *(long *)b - *(long *)a;
    return diff / labs(diff);
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argv[1] != NULL) {
        fname = argv[1];
    }

    readData(fname);

    // Syntax check
    long scores[100];
    int score_count = 0;
    for (int i = 0; i < line_count; i++) {
#ifdef DEBUG
        printf("%03d: %s\n", i + 1, lines[i]);
#endif
        if (parseLine(lines[i]) == 0) {
            long line_score = 0;
#ifdef DEBUG
            printf("%03d: %s\n", i + 1, lines[i]);
            printf("      %s\n", stack);
#endif
            for (int i = strlen(stack) - 1; i >= 0; i--) {
                line_score = 5 * line_score + closeScore(stack[i]);
            }
#ifdef DEBUG
            printf("Line score: %ld\n", line_score);
#endif
            scores[score_count++] = line_score;
        }
    }

    qsort(scores, score_count, sizeof(long), compare_long);
    printf("Middle score : %ld\n", scores[score_count / 2]);

    return EXIT_SUCCESS;
}

