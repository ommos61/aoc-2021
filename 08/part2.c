
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEBUG
#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024

// Global data information
#define UNKNOWN (-1)
typedef struct digit {
    char count;
    char mask;
    int value;
} digit;
digit digits[500][10];
digit outputs[500][4];
int count = 0;

digit makeDigit(char *cptr) {
    digit result = {0, 0, UNKNOWN};

    while ((*cptr != '\000') && (*cptr != ' ')) {
        result.mask |= (1 << (*cptr - 'a'));
        result.count++;

        cptr++;
    }

    if (result.count == 2) result.value = 1;
    if (result.count == 3) result.value = 7;
    if (result.count == 4) result.value = 4;
    if (result.count == 7) result.value = 8;
    // printf("segment count: %d\n", result.count);
    return result;
}

void convertDigits(digit *digits, int count, char *str) {
    int index = 0;
    char *cptr = str;
    while ((index < count) && (*cptr != '\000')) {
        *digits = makeDigit(cptr);

        cptr += digits->count + 1;
        digits++;
        index++;
    }
}

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

        char digits_str[LINE_LENGTH];
        char output_str[LINE_LENGTH];
        if (sscanf(line, "%[a-g ] | %[a-g ]", digits_str, output_str) == 2) {
            // Split the data in 'digits_str' and 'output_str'
            printf("'%s' --> '%s'\n", digits_str, output_str);
            convertDigits(digits[count], 10, digits_str);
            convertDigits(outputs[count], 4, output_str);
        } else if (errno != 0) {
            perror("sscanf");
        } else {
            fprintf(stderr, "Unexpected input format '%s'.\n", line);
        }

        count++;
    }

    printf("entries = %d\n", count);
    fclose(fin);
}

void makeReverse(int *reverse, digit *digits) {
    for (int i = 0; i < 10; i++) {
        if (digits[i].value != UNKNOWN) {
            reverse[digits[i].value] = i;
        }
    }
}

char maskDiff(char a, char b) {
    char result = a ^ b;

    return result;
}

int bitCount(char mask) {
    int result = 0;

    while (mask != 0) {
        if (mask & 1) {
            result++;
        }
        mask >>= 1;
    }

    return result;
}

int findValue(digit *nums, char mask) {
    int result = -1;
    for (int i = 0; i < 10; i++) {
        if (nums[i].mask == mask) {
            result = nums[i].value;
            break;
        }
    }

    return result;
}

void printDigits(digit *this) {
    for (int i = 0; i < 10; i++) {
        printf("%d ", this[i].value);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argv[1] != NULL) {
        fname = argv[1];
    }

    readData(fname);

    // Algorithm
    // 0:6, 1:2, 2:5, 3:5, 4:4, 5:5, 6:6, 7:3, 8:7, 9:6
    // 2:1, 3:7, 4:4, 5:235, 6:069, 7:8
    //  aaaa
    // b    c
    // b    c
    //  dddd
    // e    f
    // e    f
    //  gggg
    int total_sum = 0;
    for (int i = 0; i < count; i++) {
        digit *nums = digits[i];
        int reverse[10];
        makeReverse(reverse, digits[i]);
        char bd = maskDiff(nums[reverse[4]].mask, nums[reverse[1]].mask);
        char cf = nums[reverse[1]].mask;
        char b = 0, d = 0;

        int t = 0;
        while (t < 20) {
            for (int j = 0; j < 10; j++) {
                digit *this = nums + j;
                if (this->count == 5) {
                    // 2, 3, 5
                    if (bitCount(this->mask & bd) == 2) {
                        this->value = 5;
                        reverse[5] = j;
                    } else {
                        assert(bitCount(this->mask & bd) == 1);
                        d = this->mask & bd;
                        b = maskDiff(this->mask & bd, bd);

                        if (bitCount(this->mask & cf) == 1) {
                            this->value = 2;
                            reverse[2] = i;
                        } else {
                            this->value = 3;
                            reverse[3] = i;
                        }
                    }
                }
                if (this->count == 6) {
                    if (d != 0) {
                        // 0, 6, 9
                        if (maskDiff(this->mask, nums[reverse[8]].mask) == d) {
                            this->value = 0;
                            reverse[0] = j;
                        } else if (bitCount(this->mask & cf) == 1) {
                            this->value = 6;
                            reverse[6] = j;
                        } else {
                            this->value = 9;
                            reverse[9] = j;
                    }

                    }
                }
            }
            t++;
        }

//        printDigits(nums);
        int found = 0;
        for (int i = 0; i < 10; i++) {
            if (nums[i].value != UNKNOWN) found++;
        }
        assert(found == 10);

        int output_num = 0;
        for (int j = 0; j < 4; j++) {
            output_num = 10 * output_num + findValue(nums, outputs[i][j].mask);
        }
//        printf("Output number = %d\n", output_num);
        total_sum += output_num;
    }

    printf("Total output sum: %d\n", total_sum);
    return EXIT_SUCCESS;
}

