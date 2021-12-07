
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 80

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc > 1) {
        fname = argv[1];
    }

    FILE *fin = fopen(fname, "r");
    if (fin == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char line[LINE_LENGTH + 1];
    int count = 0;
    int bitcount = 0;
    int *oxygen_counts = NULL;
    int *co2_counts = NULL;
    char *oxygen_start = NULL;
    char *co2_start = NULL;
    char *oxygen_match = NULL;
    char *co2_match = NULL;
    int mem_init = 0;
    int searching = 1;
    int current_bit = 0;
    do {
        int oxygen_count = 0;
        int co2_count = 0;

        //printf("-----> current_bit = %d\n", current_bit);
        // reset to the start of the input
        fseek(fin, 0, SEEK_SET);
        count = 0;

        while (fgets(line, LINE_LENGTH, fin) != NULL) {
            // strip line ending
            if (strlen(line) != 0) line[strlen(line) - 1] = 0;

            // create the counts arrays and start strings
            if (!mem_init) {
                bitcount = strlen(line);
                printf("bitcount = %d\n", bitcount);
                oxygen_counts = malloc(bitcount * sizeof(int));
                co2_counts = malloc(bitcount * sizeof(int));
                for (int i = 0; i < bitcount; i++) {
                    oxygen_counts[i] = 0;
                    co2_counts[i] = 0;
                }
                oxygen_start = malloc(bitcount + 1);
                oxygen_start[0] = '\000';
                co2_start = malloc(bitcount + 1);
                co2_start[0] = '\000';
                oxygen_match = malloc(bitcount + 1);
                co2_match = malloc(bitcount + 1);

                mem_init = 1;
            }

            // check if it is still selected
            if (strncmp(line, oxygen_start, strlen(oxygen_start)) == 0) {
                oxygen_count += 1;
                strcpy(oxygen_match, line);
                if (line[current_bit] == '1') {
                    oxygen_counts[current_bit] += 1;
                }
            }
            if (strncmp(line, co2_start, strlen(co2_start)) == 0) {
                co2_count += 1;
                strcpy(co2_match, line);
//                printf("co2 match: line = '%s', start = '%s'\n", line, co2_start);
                if (line[current_bit] == '1') {
                    co2_counts[current_bit] += 1;
                }
            }

            count++;
        }

        assert(oxygen_count != 0);
        assert(co2_count != 0);

        int one_bits = oxygen_counts[current_bit];
        if (oxygen_count == 1) {
            oxygen_start[current_bit] = oxygen_match[current_bit];
            oxygen_start[current_bit + 1] = '\000';
        } else {
            strcat(oxygen_start, (one_bits >= oxygen_count - one_bits) ? "1" : "0");
        }

        int zero_bits = co2_count - co2_counts[current_bit];
        if (co2_count == 1) {
            co2_start[current_bit] = co2_match[current_bit];
            co2_start[current_bit + 1] = '\000';
        } else {
            strcat(co2_start, (zero_bits > co2_count - zero_bits) ? "1" : "0");
        }

//        printf("oxygen_start = '%s', count = %d, one_bits = %d\n", oxygen_start, oxygen_count, one_bits);
//        printf("co2_start = '%s', count = %d, zero_bits = %d\n", co2_start, co2_count, zero_bits);

        current_bit++;
        searching = (current_bit < bitcount);
    } while (searching);

    // Convert the strings in binary to numbers
    int oxygen = 0;
    printf("final oxygen_start = '%s'\n", oxygen_start);
    for (int i = 0; i < bitcount; i++) oxygen = 2 * oxygen + ((oxygen_start[i] == '1') ? 1 : 0);
    int co2= 0;
    printf("final co2_start = '%s'\n", co2_start);
    for (int i = 0; i < bitcount; i++) co2 = 2 * co2 + ((co2_start[i] == '1') ? 1 : 0);

    printf("lines = %d\n", count);
    printf("Oxygen generator rate = %d\n", oxygen);
    printf("CO2 scrubber rate = %d\n", co2);
    printf("answer = %d\n", oxygen * co2);
    fclose(fin);

    return EXIT_SUCCESS;
}

