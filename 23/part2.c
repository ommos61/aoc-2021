
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <time.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

// Global data information
int debug = 1; // 0 = no debug output, 1 = some extra debug output
#define ROOM_CNT 4
#define ROOM_SIZE 4
#define HALL_SIZE 11
char start[ROOM_CNT * ROOM_SIZE + HALL_SIZE + 1];
char end[ROOM_CNT * ROOM_SIZE + HALL_SIZE + 1];

// Function to read all input data to memory
void readData(char *fname) {
    FILE *fin = fopen(fname, "r");
    if (fin == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    for (unsigned int i = 0; i < array_count(start); i++) {
        start[i] = '.';
        end[i] = '.';
    }
    start[array_count(start)-1] = '\000';
    end[array_count(end)-1] = '\000';
    for (int i = 0; i < ROOM_CNT; i++) {
        for (int j = 0; j < ROOM_SIZE; j++) {
            end[i * ROOM_SIZE + j] = 'A' + i;
        }
    }

    int line_count = 0;
    char line[LINE_LENGTH];
    while (fgets(line, LINE_LENGTH, fin) != NULL) {
        // strip line ending
        if (strlen(line) != 0) line[strlen(line) - 1] = 0;

        // parse the data
        if (line_count == 2) {
            int offset = 3;
            for (unsigned int i = 0; i < strlen(line); i++) {
                if ((line[i] != ' ') && (line[i] != '#')) {
                    start[offset] = line[i];
                    offset += ROOM_SIZE;
                }
            }
        } else if (line_count == 3) {
            int offset = 0;
            for (unsigned int i = 0; i < strlen(line); i++) {
                if ((line[i] != ' ') && (line[i] != '#')) {
                    start[offset] = line[i];
                    offset += ROOM_SIZE;
                }
            }
        } else {
            // skip
        }

        line_count++;
    }
    strcpy(line, "#D#C#B#A#");
    int offset = 2;
    for (unsigned int i = 0; i < strlen(line); i++) {
        if ((line[i] != ' ') && (line[i] != '#')) {
            start[offset] = line[i];
            offset += ROOM_SIZE;
        }
    }
    strcpy(line, "#D#B#A#C#");
    offset = 1;
    for (unsigned int i = 0; i < strlen(line); i++) {
        if ((line[i] != ' ') && (line[i] != '#')) {
            start[offset] = line[i];
            offset += ROOM_SIZE;
        }
    }

    printf("lines = %d\n", line_count);
    fclose(fin);
}

//================================================
// State definition and handling
typedef struct state {
    char *map;
    long cost;
    const struct state *prev;
} state;
long state_count = 0;

state *newState(const char *map, long cost) {
    state *result = malloc(sizeof(state));
    state_count++;
    result->map = malloc(strlen(map + 1));
    strcpy(result->map, map);
    result->cost = cost;
    result->prev = NULL;

    return result;
}

//------------------------------------------------
// Move definition
typedef struct move {
    int from;
    int to;
    int distance;
} move;
state *changedState(const state *s, const struct move *m) {
    state *result = newState(s->map, s->cost);
    result->prev = s;

    assert(result->map[m->from] != '.');
    assert(result->map[m->to] == '.');
    result->map[m->to] = result->map[m->from];
    result->map[m->from] = '.';

    return result;
}

int stateCompare(const void *s1, const void *s2) {
    int result = strcmp(((state*)s1)->map, ((state*)s2)->map);
    return result;
}
int costCompare(const void *s1, const void *s2) {
    int result = ((state*)s1)->cost - ((state*)s2)->cost;
    return result;
}

void freeState(state *this) {
    free(this->map);
    free(this);
    state_count--;
}

//================================================
// Queue implementation
#define DEFAULT_QUEUE_CAP 15000
typedef struct queue {
    // items are sorted with largest first
    const void **items;
    int size;
    int end;
    int (*compare)(const void *, const void *);
} queue;
queue *queueCreate(int size, int(*compare)(const void*,const void*)) {
    queue *result = malloc(sizeof(queue));
    result->size = (size != 0) ? size : DEFAULT_QUEUE_CAP;
    result->items = malloc(result->size * sizeof(void *));
    result->end = 0;
    result->compare = compare;

    return result;
}

int queueLength(queue *q) {
    return q->end;
}

void queuePush(queue *q, const void *entry) {
    assert((queueLength(q) < q->size) && "queue overflow");
    // find the place to insert
    if (q->end == 0) {
	q->items[q->end++] = entry;
    } else {
	// shift elements
	int i;
	for (i = q->end - 1; i >= 0; i--) {
	    if (q->compare(q->items[i], entry) < 0) {
		q->items[i+1] = q->items[i];
	    } else {
		break;
	    }
	}
	// insert item
	q->items[i+1] = entry;
	q->end++;
    }
}

const void *queuePop(queue *q) {
    assert((queueLength(q) > 0) && "queue underflow");
    // the end contains the item with the lowest key
    const void *result = q->items[--q->end];

    return result;
}

//================================================
// Dictionary implementation
#define DEFAULT_DICT_CAP 200000
typedef struct dict {
    void **items;
    int size;
    int new_index;
    void *last;
    int last_index;
    int (*compare)(const void*, const void*);
} dict;
dict *dictCreate(int size, int (*compare)(const void*, const void*)) {
    dict *result = malloc(sizeof(dict));
    result->size = (size != 0) ? size : DEFAULT_DICT_CAP;
    result->items = malloc(result->size * sizeof(void *));
    result->compare = compare;
    result->new_index = 0;
    result->last = NULL;
    result->last_index = -1;

    assert(result != NULL && result->items != NULL && result->compare != NULL);
    return result;
}

void *dictGet(dict *d, void *element) {
    // this returns a pointer to an existing element, the caller can modify
    // the contents of the element
    d->last_index = -1;
    d->last = NULL;
    for (int i = 0; i < d->new_index; i++) {
	if (d->compare(d->items[i], element) == 0) {
	    d->last_index = i;
	    d->last = d->items[i];
	    break;
	}
    }
    return d->last;
}

void dictPut(dict *d, void *element) {
    // assumption is that if the element already exists, it is searched for
    // just before and last_index points to it, otherwise it is a new entry
    if (d->last_index == -1) {
	assert((d->new_index < d->size) && "dict overflow");
	d->items[d->new_index] = element;
	d->new_index++;
    }
}

//================================================
#define MOVES_MAX 256
int hall_positions[] = { 0, 1, 3, 5, 7, 9, 10 };
int room_entrances[] = { 2, 4, 6, 8 };
int distance(int hall, int room) {
    // always in the direction of hall to room
    int entrance = ROOM_CNT * ROOM_SIZE + room_entrances[room / ROOM_SIZE];
    int distance = abs(hall - entrance);
    distance +=  ROOM_SIZE - room % ROOM_SIZE;
    return distance;
}
int blocked(int hall, int room, const char *map) {
    // only check if hallway is blocked is needed
    int blocked = 0;
    // check if route is blocked
    int room_entrance = ROOM_CNT * ROOM_SIZE + room_entrances[room / ROOM_SIZE];
    int step = (hall - room_entrance) / abs(hall - room_entrance);
    for (int i = room_entrance; i != hall && !blocked; i += step) {
	if (map[i] != '.') blocked = 1;
    }
    return blocked;
}
move *getNextMove(const char *map) {
    static move moves[MOVES_MAX];
    static const char *moves_base_map = NULL;
    static int move_count = 0;
    static int current_move = 0;

    move *result = NULL;
    if ((moves_base_map != NULL) && (strcmp(map, moves_base_map) == 0)) {
	// return a previously collected move
	if (current_move < move_count) {
	    result = &moves[current_move++];
	} else {
	    moves_base_map = NULL;
	    result = NULL;
	}
    } else {
	// Create a new move set
	move_count = 0;
	current_move = 0;
	moves_base_map = map;
	// add moves from hall to room
	int hall_offset = ROOM_CNT * ROOM_SIZE;
	for (unsigned int i = 0; i < array_count(hall_positions); i++) {
	    int hall_position = hall_offset + hall_positions[i];
	    char a = map[hall_position];
	    if (a == '.') continue;
	    int room_offset = (a - 'A') * ROOM_SIZE;
	    int can_enter = 1;
	    int room_position;
	    for (int i = 0; i < ROOM_SIZE; i++) {
		room_position = room_offset + i;
		char r = map[room_position];
		if (a == r) continue;
		if (r == '.') break;
		can_enter = 0;
	    }
	    if (can_enter && !blocked(hall_position, room_position, map)) {
		moves[move_count].from = hall_position;
		moves[move_count].to = room_position;
		moves[move_count].distance = distance(hall_position, room_position);
		move_count++;
	    }
	}
	// from room to hall
	for (int i = 0; i < ROOM_CNT; i++) {
	    int room_offset = i * ROOM_SIZE;
	    int room_pos = ROOM_SIZE - 1;
	    int room_stay = 0;
	    while (room_stay < ROOM_SIZE) {
		if (map[room_offset + room_stay] != ('A' + i)) break;
		room_stay++;
	    }
	    while (room_pos >= room_stay) {
		if (map[room_offset + room_pos] != '.') {
		    break;
		}
		room_pos--;
	    }
	    if (room_pos >= room_stay) {
		int room_position = room_offset + room_pos;
		for (unsigned int j = 0; j < array_count(hall_positions); j++) {
		    int hall_position = hall_offset + hall_positions[j];
		    if (map[hall_position] == '.') {
			if (!blocked(hall_position, room_position, map)) {
			    moves[move_count].from = room_position;
			    moves[move_count].to = hall_position;
			    moves[move_count].distance = distance(hall_position, room_position);
			    move_count++;
			}
		    }
		}
	    }
	}

	result = getNextMove(map);
    }

    return result;
}

long energy(const state *s, const move *m) {
    long result = 0;
    switch (s->map[m->from]) {
    case 'A':
	result = m->distance * 1;
	break;
    case 'B':
	result = m->distance * 10;
	break;
    case 'C':
	result = m->distance * 100;
	break;
    case 'D':
	result = m->distance * 1000;
	break;
    default:
	assert(0 && "unreachable");
	break;
    }

    return result;
}

state *dijkstra(const char *start, const char *end) {
    dict *seen = dictCreate(0, stateCompare);
    queue *q = queueCreate(0, costCompare);
    state *startState = newState(start, 0);
    state *endState = newState(end, LONG_MAX);
    dictPut(seen, startState);
    queuePush(q, startState);

    long states_handled = 0;
    while (queueLength(q) > 0) {
	const state *current = queuePop(q);
	if (debug) printf("%s (%ld)\n", current->map, current->cost);
	states_handled++;
	if (stateCompare(current, endState) == 0) {
	    if (current->cost < endState->cost) {
		endState->cost = current->cost;
		endState->prev = current->prev;
	    }
	    if (debug) printf("States handled: %ld\n", states_handled);
	    if (debug) printf("States still queued: %d\n", queueLength(q));
	    break;
	} else {
	    move *toNext;
	    while ((toNext = getNextMove(current->map)) != NULL) {
		state *nextState0 = changedState(current, toNext);
		state *nextState1 = dictGet(seen, nextState0);
		long next_cost = current->cost + energy(current, toNext);
		if (debug) printf("Next state: %s (%ld)\n", nextState0->map, next_cost);
		if (nextState1 == NULL) {
		    nextState0->cost = next_cost;
		    dictPut(seen, nextState0);
		    queuePush(q, nextState0);
		} else {
		    if (next_cost < nextState1->cost) {
			nextState1->cost = next_cost;
			nextState1->prev = current;
			nextState0->cost = next_cost;
			nextState0->prev = current;
			queuePush(q, nextState0);
		    } else {
			freeState(nextState0);
		    }
		}
	    }
	    char buf[10];
	    if (debug) fgets(buf, 5, stdin);
	    //return endState;
	}
    }

    return endState;
}

void printRoom(char *map) {
    for (int i = 0; i < HALL_SIZE + 2; i++) printf("#");
    printf("\n");
    printf("#%s#\n", map + ROOM_CNT * ROOM_SIZE);
    for (int i = ROOM_SIZE - 1; i >= 0; i--) {
	if (i == ROOM_SIZE - 1) printf("###"); else printf("  #");
	for (int j = 0; j < ROOM_CNT; j++) {
	    printf("%c#", map[j * ROOM_SIZE + i]);
	}
	if (i == ROOM_SIZE - 1) printf("##"); printf("\n");
    }
    printf("  #########\n");
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc != 1) {
        fname = argv[1];
    }

    readData(fname);

    clock_t start_time, end_time;

    printRoom(start);
    printRoom(end);

    start_time = clock();
    debug = 0;
    state *end_state = dijkstra(start, end);
    end_time = clock();

    printf("Minimal cost was %ld\n", end_state->cost);
    printf("LONG_MAX (inf.): %ld\n", LONG_MAX);
    printf("Time: %.3f seconds\n", (float)(end_time - start_time) / CLOCKS_PER_SEC);
    printf("States allocated: %ld\n", state_count);

    const state *current = end_state;
    while (current != NULL) {
	printf("%s (%ld)\n", current->map, current->cost);
	//printRoom(current->map, ROOM_SIZE);
	current = current->prev;
    }

    return EXIT_SUCCESS;
}

