
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024

// Global data information
typedef struct fish {
    int timer;
    struct fish *next;
} fish;
struct {
    int count;
    fish *first;
    fish *last;
} fishes = {0, NULL, NULL};

void addFish(int timer) {
    // create the new fish
    fish *new_fish = malloc(sizeof(fish));
    new_fish->timer = timer;
    new_fish->next = NULL;

    // add it to the end of the list
    if (fishes.first == NULL) {
        fishes.first = new_fish;
        fishes.last = new_fish;
    } else {
        fishes.last->next = new_fish;
        fishes.last = new_fish;
    }
    fishes.count++;
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

    printf("fishes = %d\n", fishes.count);
    fclose(fin);
}

void printFishes(void) {
    fish *this = fishes.first;
    while (this != NULL) {
        printf("%d%s", this->timer, (this->next != NULL) ? "," : "");
        this = this->next;
    }
}

void evolveFishes(void) {
    // Update the fish polulation to the next day
    int new_fish = 0;
    fish *this = fishes.first;
    while (this != NULL) {
        if (this->timer > 0) {
            this->timer--;
        } else {
            this->timer = 6;
            new_fish++;
        }
        this = this->next;
    }

    for (int i = 0; i < new_fish; i++) {
        addFish(8);
    }
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc != 1) {
        fname = argv[1];
    }

    readData(fname);
    printf("Inital state: ");
    printFishes();
    printf("\n");

    // Evolve the fish for some days
    int day = 0;
    while (day < 80) {
        day++;

        evolveFishes();

        if (day <= 18) {
            printf("After %2d day%s: ", day, (day > 1) ? "s" : " ");
            printFishes();
            printf("\n");
        }
    }

    printf("Fish count after %d days: %d\n", day, fishes.count);

    return EXIT_SUCCESS;
}

