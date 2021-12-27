
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

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

long p1Wins[2][15][15][25][25];
long p2Wins[2][15][15][25][25];
int visited[2][15][15][25][25];

void calculateWins(int p, int pos1, int pos2, int score1, int score2, long *wins1, long *wins2) {
    if (visited[p][pos1][pos2][score1][score2]) {
        *wins1 = p1Wins[p][pos1][pos2][score1][score2];
        *wins2 = p2Wins[p][pos1][pos2][score1][score2];
    } else {
        if (score1 >= 21) {
            *wins1 = 1;
            *wins2 = 0;
        } else if (score2 >= 21) {
            *wins1 = 0;
            *wins2 = 1;
        } else {
            if (p == 0) {
                *wins1 = 0;
                *wins2 = 0;
                for (int r1 = 1; r1 <= 3; r1++) {
                    for (int r2 = 1; r2 <= 3; r2++) {
                        for (int r3 = 1; r3 <= 3; r3++) {
                            long wins1_sub, wins2_sub;
                            int newpos = (pos1 + r1 + r2 + r3 - 1) % 10 + 1;
                            calculateWins(1 - p, newpos, pos2, score1 + newpos, score2, &wins1_sub, &wins2_sub);
                            *wins1 += wins1_sub;
                            *wins2 += wins2_sub;
                        }
                    }
                }
            } else {
                *wins1 = 0;
                *wins2 = 0;
                for (int r1 = 1; r1 <= 3; r1++) {
                    for (int r2 = 1; r2 <= 3; r2++) {
                        for (int r3 = 1; r3 <= 3; r3++) {
                            long wins1_sub, wins2_sub;
                            int newpos = (pos2 + r1 + r2 + r3 - 1) % 10 + 1;
                            calculateWins(1 - p, pos1, newpos, score1, score2 + newpos, &wins1_sub, &wins2_sub);
                            *wins1 += wins1_sub;
                            *wins2 += wins2_sub;
                        }
                    }
                }
            }
        }
        visited[p][pos1][pos2][score1][score2] = 1;
        p1Wins[p][pos1][pos2][score1][score2] = *wins1;
        p2Wins[p][pos1][pos2][score1][score2] = *wins2;
    }
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc != 1) {
        fname = argv[1];
    }

    readData(fname);

    // implement algorithm
    long wins1 = 0;
    long wins2 = 0;
    calculateWins(0,
        player_positions[0], player_positions[1],
        player_scores[0], player_scores[1],
        &wins1, &wins2);
    printf("Player 1 wins: %ld\n", wins1);
    printf("Player 2 wins: %ld\n", wins2);

    printf("Part 2 answer: %ld\n", MAX(wins1,  wins2));

    return EXIT_SUCCESS;
}


