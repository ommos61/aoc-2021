
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

// Global data information
int debug = 1; // 0 = no debug output, 1 = some extra debug output
#define PLAYERS_MAX 2
int player_positions[PLAYERS_MAX];
int player_scores[PLAYERS_MAX];
int player_count = 0;
int die_rolls = 0;

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

        int player = 0;
        int position = 0;
        if (sscanf(line, "Player %d starting position: %d", &player, &position) == 2) {
            assert(player <= PLAYERS_MAX);
            assert(player_count <= PLAYERS_MAX);
            player_positions[player - 1] = position;
        } else if (errno != 0) {
            perror("sscanf");
        } else {
            fprintf(stderr, "Unexpected input format '%s'.\n", line);
        }

        player_count++;
    }

    printf("Players = %d\n", player_count);
    fclose(fin);
}

int roll(void) {
    static int state = 0;
    int current_roll = state + 1;

    state = (state + 1) % 100;
    die_rolls++;

    return current_roll;
}

int playerMove(int player, int roll_total) {
    int new_position = (player_positions[player] - 1 + roll_total) % 10 + 1;

    player_positions[player] = new_position;
    player_scores[player] += new_position;

    return player_scores[player];
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc != 1) {
        fname = argv[1];
    }

    readData(fname);

    // implement algorithm
    int finished = 0;
    int player_turn = 0;
    while (! finished) {
        int roll_total = roll() + roll() + roll();
        int new_score = playerMove(player_turn, roll_total);
        if (new_score >= 1000) {
            finished = 1;
        }
        printf("Player %d rolls %d and moves to space %d for a total core of %d.\n",
            player_turn + 1, roll_total, player_positions[player_turn], new_score);
        player_turn = (player_turn + 1) % player_count;
    }

    printf("Die has rolled %d times.\n", die_rolls);
    printf("Losing player has %d points.\n", player_scores[player_turn]);

    printf("Part 1 answer: %d\n", die_rolls * player_scores[player_turn]);

    return EXIT_SUCCESS;
}

