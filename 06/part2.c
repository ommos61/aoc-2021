
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024

// Global data information
#define MAX_TIMER 8
long fishes[MAX_TIMER + 1] = {0};
long fish_count = 0;

void addFish(int timer) {
    // add the new fish
    fishes[timer]++;

    // update the total count
    fish_count++;
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

        char *cptr = line;
        while (cptr < line + strlen(line)) {
            int timer = 0;
            int nread = sscanf(cptr, "%d", &timer);
            if ((nread == EOF) || (nread != 1)) {
                perror("sscanf");
                exit(EXIT_FAILURE);
            }
            
            addFish(timer);

            while (*cptr && (*cptr != ',')) cptr++;
            if (*cptr == ',') cptr++;
        }
    }

    printf("fishes = %ld\n", fish_count);
    fclose(fin);
}

void evolveFishes(void) {
    // Update the fish polulation to the next day
    long new_fish = fishes[0];
    for (int i = 1; i < MAX_TIMER + 1; i++) {
        fishes[i - 1] = fishes[i];
    }
    fishes[6] += new_fish;
    fishes[8] = new_fish;
    
    fish_count += new_fish;
}

void printFishes(void) {
    for (int i = 0; i < MAX_TIMER + 1; i++) {
        printf("%d:%ld ", i, fishes[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc != 1) {
        fname = argv[1];
    }

    printf("Size of 'int' is %lu bytes\n", sizeof(int));
    printf("Size of 'long' is %lu bytes\n", sizeof(long));
    printf("LONG_MAX is %ld\n", LONG_MAX);

    readData(fname);
    printf("Inital state: %ld fishes\n", fish_count);

    // Evolve the fish for some days
    int day = 0;
    while (day < 256) {
        day++;
        evolveFishes();

        if (day <= 18) printf("After %2d day%s: %ld fishes\n", day, (day > 1) ? "s" : " ", fish_count);
        if (day == 80) printf("Fish count after %d days: %ld fishes\n", day, fish_count);
        if (day == 256) { printFishes(); printf("Fish count after %d days: %ld fishes\n", day, fish_count); }
    }

    return EXIT_SUCCESS;
}

