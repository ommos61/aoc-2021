
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define DEBUG
#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024

// Global data information
char *template = NULL;
#define RULES_MAX 1024
typedef struct rule {
    char pair[2];
    char insert;
} rule;
rule rules[RULES_MAX];
int rule_count;

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

        if (template == NULL) {
            template = malloc(strlen(line) + 1);
            strcpy(template, line);
        } else {
            if (strlen(line) > 0) {
                char pair[10], insert[10];
                if (sscanf(line, "%[A-Z] -> %[A-Z]", pair, insert) == 2) {
                    rules[rule_count].pair[0] = pair[0];
                    rules[rule_count].pair[1] = pair[1];
                    rules[rule_count].insert = insert[0];
                    rule_count++;
                } else if (errno != 0) {
                    perror("sscanf");
                } else {
                    fprintf(stderr, "Unexpected input format '%s'.\n", line);
                }
            }
        }
    }

    printf("Template: %s\n", template);
    printf("Rule count = %d\n", rule_count);
    fclose(fin);
}

char getInsert(char a, char b) {
    char result = 0;

    for (int i = 0; i < rule_count; i++) {
        if ((rules[i].pair[0] == a) && (rules[i].pair[1] == b)) {
            result = rules[i].insert;
            break;
        }
    }

//    printf("  %c%c -> %c\n", a, b, (result != 0) ? result : '?');
    return result;
}

char *performStep(char *template) {
    char *result = malloc(2 * strlen(template));

    char *cptr = result;
    for (int i = 0; i < strlen(template) - 1; i++) {
        *cptr++ = template[i];
        char c;
        if ((c = getInsert(template[i], template[i + 1])) != 0) {
            *cptr++ = c;
        }
    }
    *cptr++ = template[strlen(template) - 1];
    *cptr++ = '\000';

    return result;
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argv[1] != NULL) {
        fname = argv[1];
    }

    readData(fname);

    // perform some insertion steps
    for (int i = 0; i < 10; i++) {
        char *new = performStep(template);
        free(template);
        template = new;
        printf("Length after step %d: %ld\n", i + 1, strlen(template));
        //printf("After step %d: %s\n", i + 1, template);
    }

    int counts[26];
    memset(counts, 0, 26 * sizeof(int));
    for (int i = 0; i < strlen(template); i++) {
        counts[template[i] - 'A']++;
    }
    int min = INT_MAX;
    int max = 0;
    for (int i = 0; i < 26; i++) {
        if (counts[i] != 0) {
            if (counts[i] > max) max = counts[i];
            if (counts[i] < min) min = counts[i];
        }
    }

    printf("min, max = %d, %d\n", min, max);
    printf("Answer is %d\n", max - min);
    free(template);

    return EXIT_SUCCESS;
}


