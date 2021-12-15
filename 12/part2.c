
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

//#define DEBUG
#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024

// Global data information
#define NODE_MAX 50
#define TYPE_SMALL 0
#define TYPE_LARGE 1
typedef struct node {
    char *name;
    int type;
    struct node *neighbors[NODE_MAX];
    int neighbor_count;
    int visit_count;
} node;
node nodes[NODE_MAX];
int node_count = 0;

#define CONNECTIONS_MAX 50
typedef struct connection {
    char *one;
    char *two;
} connection;
connection connections[CONNECTIONS_MAX];
int connection_count;

// Function to read all input data to memory
void readData(char *fname) {
    FILE *fin = fopen(fname, "r");
    if (fin == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int line_count = 0;
    char line[LINE_LENGTH];
    while (fgets(line, LINE_LENGTH, fin) != NULL) {
        // strip line ending
        if (strlen(line) != 0) line[strlen(line) - 1] = 0;

        char one[LINE_LENGTH], two[LINE_LENGTH];
        if (sscanf(line, "%[A-Za-z]-%[A-Za-z]", one, two) == 2) {
            connections[connection_count].one = malloc(strlen(one) + 1);
            connections[connection_count].two = malloc(strlen(two) + 1);
            strcpy(connections[connection_count].one, one);
            strcpy(connections[connection_count].two, two);
            connection_count++;
        } else if (errno != 0) {
            perror("sscanf");
        } else {
            fprintf(stderr, "Unexpected input format '%s'.\n", line);
        }

        line_count++;
    }

    printf("lines = %d\n", line_count);
    fclose(fin);
}

void printConnections(void) {
    printf("Connections:\n");
    for (int i = 0; i < connection_count; i++) {
        printf("  %s -> %s\n", connections[i].one, connections[i].two);
    }
}

void printNodes(void) {
    printf("Nodes:\n");
    for (int i = 0; i < node_count; i++) {
        printf("  %s (%s): ", nodes[i].name, (nodes[i].type == TYPE_SMALL) ? "small" : "large");
        for (int j = 0; j < nodes[i].neighbor_count; j++) {
            printf("%s ", nodes[i].neighbors[j]->name);
        }
        printf("\n");
    }
}

node *findCreateNode(char *name) {
    node *result = NULL;
    for (int i = 0; i < node_count; i++) {
        if (strcmp(nodes[i].name, name) == 0) {
            result = nodes + i;
        }
    }
    if (result == NULL) {
        nodes[node_count].name = malloc(strlen(name) + 1);
        strcpy(nodes[node_count].name, name);
        nodes[node_count].type = islower(name[0]) ? TYPE_SMALL : TYPE_LARGE;
        nodes[node_count].neighbor_count = 0;

        result = nodes + node_count;
        node_count++;
    }
    return result;
}

void addNeighbor(node *from, node *to) {
    node *neighbor = NULL;
    for (int i = 0; i < from->neighbor_count; i++) {
        if (from->neighbors[i] == to) {
            neighbor = to;
        }
    }
    if (neighbor == NULL) {
        from->neighbors[from->neighbor_count++] = to;
    }
}

void createNodes(void) {
    for (int i = 0; i < connection_count; i++) {
        node *node1 = findCreateNode(connections[i].one);
        node *node2 = findCreateNode(connections[i].two);
        addNeighbor(node1, node2);
        addNeighbor(node2, node1);
    }
}

void printRoute(char *prefix, int node_count, node *route[]) {
    if (prefix) printf("%s ", prefix);
    for (int i = 0; i < node_count; i++) {
        printf("%s%s", route[i]->name, (i != node_count - 1) ? "," : "");
    }
    printf("\n");
}

void printAddRoute(node *this, int route_count, node *route[]) {
    char prefix[30];
    sprintf(prefix, "  Add '%s' to:", this->name);
    printRoute(prefix, route_count, route);
}

int isValidTarget(node *this, int route_count, node *route[]) {
//    printAddRoute(this, route_count, route);
    if (strcmp(this->name, "start") == 0) return 0;
    if (strcmp(this->name, "end") == 0) return 0;
    if (this->type == TYPE_LARGE) return 1;

    // (neighbor->type == TYPE_SMALL)
    // clear the visit_count for all nodes
    for (int i = 0; i < node_count; i++) nodes[i].visit_count = 0;
    // increment the visit_count for all elements in the route
    node *visited_twice = NULL;
    for (int i = 0; i < route_count; i++) {
        // current node on route
        node *c = route[i];
        c->visit_count += 1;

        if (c->type == TYPE_SMALL) {
            if (c->visit_count == 2) {
                //printAddRoute(this, route_count, route);
                assert(visited_twice == NULL);
                visited_twice = c;
            }
            // invalid check
            if ((visited_twice == this) ||
                ((visited_twice != NULL) && (this->visit_count == 1))) {
                return 0;
                break;
            }
        }
    }

    return 1;
}

int findRoute(node *from, node *to, int node_count, node *route[]) {
    int result = 0;

    route[node_count++] = from;
//    printRoute("Current route:", node_count, route);
    for (int i = 0; i < from->neighbor_count; i++) {
        if (from->neighbors[i] == to) {
            // Found a route
            route[node_count++] = to;
#ifdef DEBUG
            printRoute("Found:", node_count, route);
#endif
            result += 1;
            // back-up to route one
            node_count--;
        } else {
            node *neighbor = from->neighbors[i];
            if (isValidTarget(neighbor, node_count, route)) {
                result += findRoute(from->neighbors[i], to, node_count, route);
            }
        }
    }

    return result;
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argv[1] != NULL) {
        fname = argv[1];
    }

    readData(fname);
    printConnections();
    createNodes();
    printNodes();
    printf("==========\n");

    // Find routes from 'start' to 'end'
    node *route[200];
    int count = findRoute(findCreateNode("start"), findCreateNode("end"), 0, route);
    printf("Route count: %d\n", count);

    return EXIT_SUCCESS;
}

