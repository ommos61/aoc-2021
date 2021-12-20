
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024

// Global data information
int debug = 1; // 0 = no debug output, 1 = some extra debug output
#define NODE_PAIR 0
#define NODE_NUMBER 1
typedef struct node {
    int type;
    struct node *left;
    struct node *right;
    int number;
} node;
char *lines[100];
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

void parseLine(node **tree, const char *line, int *offset) {
    *tree = malloc(sizeof(node));

    if (line[*offset] == '[') {
        (*offset)++;
        (*tree)->type = NODE_PAIR;
        parseLine(&(*tree)->left, line, offset);
        assert(line[*offset] == ',');
        (*offset)++;
        parseLine(&(*tree)->right, line, offset);
        assert(line[*offset] == ']');
        (*offset)++;
    } else {
        assert((line[*offset] >= '0') && (line[*offset] <= '9'));
        (*tree)->type = NODE_NUMBER;
        (*tree)->number = line[*offset] - '0';
        (*offset)++;
    }
}

void printTree(node *tree, int level) {
    if (tree->type == NODE_PAIR) {
        printf("[");
        printTree(tree->left, level + 1);
        printf(",");
        printTree(tree->right, level + 1);
        printf("]");
    } else if (tree->type == NODE_NUMBER) {
        printf("%d", tree->number);
    } else {
        printf("!!! ERROR: unknown node type !!!");
    }
    if (level == 0) printf("\n");
}

node *add(node *val1, node *val2) {
    assert(val1 != NULL);
    assert(val2 != NULL);

    node *result = malloc(sizeof(node));
    result->type = NODE_PAIR;
    result->left = val1;
    result->right = val2;

    return result;
}

node *last_number = NULL;
int add_right = -1;
int explode(node *tree, int level) {
    int changed = 0;
    if (level == 0) { last_number = NULL; add_right = -1; }

    if (tree->type == NODE_PAIR) {
        if (level == 4) {
            if (debug) { printf("%d: explode ", level); printTree(tree, level); printf("\n"); }

            // perform the explode
            if (last_number != NULL) {
                assert(tree->left->type == NODE_NUMBER);
                last_number->number += tree->left->number;
            }
            add_right = tree->right->number;
            tree->type = NODE_NUMBER;
            tree->number = 0;
            free(tree->left);
            free(tree->right);
            changed = 1;
        } else {
            // if (debug) printf("-> level %d\n", level);
            changed = explode(tree->left, level + 1);
            if (changed) {
                node *start = tree->right;
                while (add_right != -1) {
                    if (start->type == NODE_NUMBER) {
                        start->number += add_right;
                        add_right = -1;
                    } else {
                        start = start->left;
                    }
                }
            } else {
                changed = explode(tree->right, level + 1);
            }
        }
    } else {
        last_number = tree;
        if (add_right != -1) {
            tree->number += add_right;
            add_right = -1;
        }
    }

    return changed;
}

int split(node *tree, int level) {
    int changed = 0;

    if (tree->type == NODE_NUMBER) {
        if (tree->number >= 10) {
            if (debug) printf("%d: split %d\n", level, tree->number);

            tree->type = NODE_PAIR;
            tree->left = malloc(sizeof(node));    
            tree->left->type = NODE_NUMBER;
            tree->left->number = tree->number / 2;
            tree->right = malloc(sizeof(node));    
            tree->right->type = NODE_NUMBER;
            tree->right->number = (tree->number + 1) / 2;
            changed = 1;
        }
    } else {
        changed = split(tree->left, level + 1);
        if (! changed) changed = split(tree->right, level + 1);
    }

    return changed;
}

int reduce(node *tree) {
    int changed = 0;

    changed = explode(tree, 0);
    if (! changed) {
        changed = split(tree, 0);
    }

    return changed;
}

int magnitude(node *tree) {
    int result = 0;
    if (tree->type == NODE_NUMBER) {
        result =  tree->number;
    } else {
        result = 3 * magnitude(tree->left);
        result += 2 * magnitude(tree->right);
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

    // Implement algorithm
    int max_magnitude = 0;
    int max_index1 = -1;
    int max_index2 = -1;
    node *max_sum = NULL;
    for (int i = 1; i < line_count; i++) {
        for (int j = 0; j < line_count; j++) {
            if (j != i) {
                node *value1 = NULL;
                int offset = 0;
                parseLine(&value1, lines[i], &offset);
                if (debug){ printf("  "); printTree(value1, 0); }

                node *value2 = NULL;
                offset = 0;
                parseLine(&value2, lines[j], &offset);
                if (debug){ printf("  "); printTree(value2, 0); }

                node *sum = add(value1, value2);
                debug = 0;
                if (debug) { printf("= "); printTree(sum, 0); }
                while (reduce(sum)) {
                    if (debug) {
                        printf("->");
                        printTree(sum, 0);
                    }
                }
                { printf("= "); printTree(sum, 0); }
                printf("\n");

                int mag = magnitude(sum);
                if (mag > max_magnitude) {
                    max_magnitude = mag;
                    max_index1 = i;
                    max_index2 = j;
                    max_sum = sum;
                }
            }
        }
    }
    printf("  %s\n", lines[max_index1]);
    printf("+ %s\n", lines[max_index2]);
    printf("= "); printTree(max_sum, 0);
    printf("Maximum magnitude: %d\n", max_magnitude);

    return EXIT_SUCCESS;
}

