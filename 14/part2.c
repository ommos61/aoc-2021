
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
#define NUM_CAPS 26
char rules[NUM_CAPS][NUM_CAPS];
#define mkIndex(c) ((c) - 'A')

// Function to read all input data to memory
void readData(char *fname) {
    FILE *fin = fopen(fname, "r");
    if (fin == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char line[LINE_LENGTH];
    int rule_count = 0;
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
                    rules[mkIndex(pair[0])][mkIndex(pair[1])] = insert[0];
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

long pair_counts[2][NUM_CAPS][NUM_CAPS];
int current_set = 0;

void initCounts(char *template) {
    for (unsigned int i = 0; i < strlen(template) - 1; i++) {
        printf("init for '%c%c'\n", template[i], template[i + 1]);
        pair_counts[current_set][template[i] - 'A'][template[i + 1] - 'A']++;
    }
    long cnt = 0;
    for (int i = 0; i < NUM_CAPS * NUM_CAPS; i++) cnt += pair_counts[current_set][0][i];
    printf("Init: %ld pairs\n", cnt);
}

void printSet(int set_idx) {
    printf("   ");
    for (int j = 0; j < NUM_CAPS; j++) printf("  %c", j + 'A');
    printf("\n");
    for (int i = 0; i < NUM_CAPS; i++) {

        printf("  %c ", (i + 'A'));
        for (int j = 0; j < NUM_CAPS; j++) {
            char c = (rules[i][j] != '\000') ? rules[i][j] : ' ';
            printf("%2ld%c", pair_counts[set_idx][i][j], c);
        }
        printf("\n");
    }
}

long performStep() {
    long result = 0;

    int new_set = (current_set + 1) % 2;
    for (int i = 0; i < NUM_CAPS * NUM_CAPS; i++) pair_counts[new_set][0][i] = 0;

    for (int row = 0; row < NUM_CAPS; row++) {
        for (int col = 0; col < NUM_CAPS; col++) {
            long cnt = pair_counts[current_set][row][col];
            if (cnt != 0) {
                if (rules[row][col] != '\000') {
                    int i1 = row;
                    int i2 = mkIndex(rules[row][col]);
                    int i3 = col;
                    pair_counts[new_set][i1][i2] += cnt;
                    pair_counts[new_set][i2][i3] += cnt;
                    result += 2 * cnt;
                }
            }
        }
    }

    current_set = new_set;
    return result + 1;
}

long getCount(int idx) {
    long result = 0;

    for (int i = 0; i < NUM_CAPS; i++) {
        result += pair_counts[current_set][idx][i];
    }
    //printf("Template last: %c\n", template[strlen(template) - 1]);
    if (idx == (template[strlen(template) - 1] - 'A')) {
        result++;
    }

    return result;
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc != 1) {
        fname = argv[1];
    }

    readData(fname);

    // perform some insertion steps
    initCounts(template);
    printf("Length after step 0: %ld\n", strlen(template));
    //printSet(current_set);
    long min, max;
    for (int i = 0; i < 40; i++) {
        long char_count = performStep();

        printf("Length after step %d: %ld\n", i + 1, char_count);

        min = LONG_MAX;
        max = 0;
        printf("  ");
        long sum = 0;
        for (int i = 0; i < NUM_CAPS; i++) {
            long count = getCount(i);
            if (count != 0) {
                if (count > max) max = count;
                if (count < min) min = count;
//                printf("%c: %ld, ", i + 'A', count);
                sum += count;
            }
        }
//        printf("\n");
        printf("Total count: %ld\n", sum);

        //printSet(current_set);
    }
    
    printf("min, max = %ld, %ld\n", min, max);
    printf("Answer is %ld\n", max - min);
    printf("LONG_MAX = %ld\n", LONG_MAX);
    free(template);

    return EXIT_SUCCESS;
}



