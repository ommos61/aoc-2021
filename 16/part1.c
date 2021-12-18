
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 4096

// Global data information
int debug = 0; // 0 = no debug info printed, 1 = debug info printed
char *lines[10];
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

int hex2dec(char c) {
    if (c <= '9') return c - '0';
    return 10 + c - 'A';
}

int bitnr2mask(int offset) {
    int nibble_offset = offset % 4;
    int mask = 1 << (3 - nibble_offset);

    return mask;
}

void printBits(const char *input, int offset) {
    for (unsigned int i = 0; i < strlen(input); i++) {
        printf("   %c ", input[i]);
    }
    printf("\n");
    for (unsigned int i = 0; i < strlen(input); i++) {
        char buffer[5] = {0};
        int val = hex2dec(input[i]);
        int i = 3;
        while (i >= 0) {
            buffer[i] = (val & 1) ? '1' : '0';
            val >>= 1;
            i--;
        }
        printf("%4s ", buffer);
    }
    printf("\n");
    for (int i = 0; i < offset; i++) {
        printf(" ");
        if (i % 4 == 3) printf(" ");
    }
    printf("^\n");
}

int getBits(const char *input, int offset, int bit_count) {
    int result = 0;

    if (bit_count != 0) {
        int ch_index = offset / 4;
        int val = hex2dec(input[ch_index]);
        result = ((val & bitnr2mask((offset)) ? 1 << (bit_count - 1) : 0) + getBits(input, offset + 1, bit_count - 1));
    }

    return result;
}

int sum_versions = 0;
int parseLine(const char *input, int offset) {
    int index = offset;
    if (debug) printBits(input, index);

    int version = getBits(input, index, 3);
    int type_id = getBits(input, index + 3, 3);
    index += 6;
    sum_versions += version;
    printf("  Version: %d, type ID: %d\n", version, type_id);
    if (debug) printBits(input, index);
    if (type_id == 4) {
        // a literal number
        int val = 0;
        int bits = 0;
        do {
            bits = getBits(input, index, 5);
            val = 16 * val + (bits & 0x0F);
            index += 5;
        } while ((bits & 0x10) != 0);
        printf("Literal: %d\n", val);
    } else {
        // an operator with sub-packets
        int len_type = getBits(input, index, 1);
        index += 1;
        if (len_type == 0) {
            int len = getBits(input, index, 15);
            index += 15;
            printf("Length type: %d (%d bit(s))\n", len_type, len);
            int new_index = index;
            while (new_index < index + len) {
                new_index = parseLine(input, new_index);
            }
            assert(new_index == index + len);
            index = new_index;
        } else {
            int cnt = getBits(input, index, 11);
            index += 11;
            printf("Length type: %d (%d item(s))\n", len_type, cnt);
            for (int i = 0; i < cnt; i++) {
                index = parseLine(input, index);
            }
        }
    }

    return index;
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc != 1) {
        fname = argv[1];
    }

    readData(fname);

    // Parse the input line(s)
    for (int i = 0; i < line_count; i++) {
        printf("Line %d:\n", i + 1);
        if (debug) printf("--> %s\n", lines[i]);
        sum_versions = 0;
        int index = parseLine(lines[i], 0);
        printf("----> Versions sum: %d\n", sum_versions);
        // TODO: verify that only 0 bits are present after 'index'
        assert(index != 0);
    }

    return EXIT_SUCCESS;
}

