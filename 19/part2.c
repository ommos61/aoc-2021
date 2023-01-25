
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#pragma clang optimize on

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

// Global data information
int debug = 1; // 0 = no debug output, 1 = some extra debug output
#define POINTS_MAX 100
typedef struct point {
    int x;
    int y;
    int z;
} point;
typedef struct scanner {
    int id;
    point *pos;
    point *beacons;
    int beacon_count;
} scanner;
scanner scanners[50];
int scanner_count = 0;

// Function to read all input data to memory
void readData(char *fname) {
    FILE *fin = fopen(fname, "r");
    if (fin == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char line[LINE_LENGTH];
    point points[POINTS_MAX];
    int point_count = 0;
    int scanner_id = -1;
    while (fgets(line, LINE_LENGTH, fin) != NULL) {
        // strip line ending
        if (strlen(line) != 0) line[strlen(line) - 1] = 0;

        int x, y, z;
        if (sscanf(line, "--- scanner %d ---", &scanner_id) == 1) {
            // start beacon list for scanner 'scanner_id'
            scanners[scanner_count].id = scanner_id;
            scanner_count++;
            point_count = 0;
        } else if (sscanf(line, "%d,%d,%d", &x, &y, &z) == 3) {
            // store the beacon location
            points[point_count].x = x;
            points[point_count].y = y;
            points[point_count].z = z;
            point_count++;
        } else if (strlen(line) == 0) {
            // skip empty lines
            printf("Saving data for scanner %d\n", scanner_id);
            scanners[scanner_id].beacons = malloc(point_count * sizeof(point));
            memcpy(scanners[scanner_id].beacons, points, point_count * sizeof(point));
            scanners[scanner_id].beacon_count = point_count;
        } else if (errno != 0) {
            perror("sscanf");
        } else {
            fprintf(stderr, "Unexpected input format '%s'.\n", line);
        }
    }
    if (point_count != 0) {
        printf("Saving data for scanner %d\n", scanner_id);
        scanners[scanner_id].beacons = malloc(point_count * sizeof(point));
        memcpy(scanners[scanner_id].beacons, points, point_count * sizeof(point));
        scanners[scanner_id].beacon_count = point_count;
    }

    printf("Scanners = %d\n", scanner_count);
    fclose(fin);
}

point mkPoint(int x, int y, int z) {
    point result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

point pointsDiff(point *p1, point *p2) {
    point result =  {0};
    result.x = p1->x - p2->x;
    result.y = p1->y - p2->y;
    result.z = p1->z - p2->z;

    return result;
}

int pointsEqual(point *p1, point *p2) {
    if (p1->x != p2->x) return 0;
    if (p1->y != p2->y) return 0;
    if (p1->z != p2->z) return 0;
    return 1;
}

void printPoint(point *p) {
    if (p != NULL)
        printf("(%d, %d, %d)", p->x, p->y, p->z);
    else
        printf("()");
}

void printScanner(scanner *s) {
    if (s != NULL) {
        printf("{ id: %d,\n", s->id);
        printf("  pos: "); printPoint(s->pos); printf("\n");
        for (int i = 0; i < s->beacon_count; i++) {
            printf("  beacon[%d]: ", i); printPoint(&s->beacons[i]); printf("\n");
        }
        printf("}\n");
    } else printf("{...}\n");
}

int isScannerBeacon(scanner *s, point *p) {
    for (int i = 0; i < s->beacon_count; i++) {
        if (pointsEqual(&s->beacons[i], p)) {
            return 1;
        }
    }

    return 0;
}

int countCommonBeacons(scanner *s1, scanner *s2) {
    int count = 0;

    for (int i = 0; i < s1->beacon_count; i++) {
        if (isScannerBeacon(s2, &s1->beacons[i])) {
            count++;
        }
    }

    return count;
}

void moveScanner(scanner *s, point p) {
    // which means, move all of it's beacons
    for (int i = 0; i < s->beacon_count; i++) {
        s->beacons[i].x += p.x;
        s->beacons[i].y += p.y;
        s->beacons[i].z += p.z;
    }
}

void swap(int *i1, int *i2) {
    int t = *i1;
    *i1 = *i2;
    *i2 = t;
}

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2
void rotatePoint(point *p, int axis) {
    switch (axis) {
    case AXIS_X:
        swap(&p->y, &p->z);
        p->z = -p->z;
        break;
    case AXIS_Y:
        swap(&p->x, &p->z);
        p->z = -p->z;
        break;
    case AXIS_Z:
        swap(&p->x, &p->y);
        p->y = -p->y;
        break;
    default:
        assert((axis >= AXIS_X) && (axis <= AXIS_Z));
        break;
    }
}

void rotateScanner(scanner *s, int axis) {
    // which means, rotate beacon all points around the scanner
    for (int i = 0; i < s->beacon_count; i++) {
        rotatePoint(s->beacons + i, axis);
    }
}

int countMatchingBeaconsByOffset(scanner *s1, scanner *s2) {
    int match_count = 0;

    for (int i = 0; i < s1->beacon_count; i++) {
        for (int j = 0; j < s2->beacon_count; j++) {
            // Difference between beacons from different scanners
            point diff = pointsDiff(&s1->beacons[i], &s2->beacons[j]);
            // Move the scanner
            moveScanner(s2, diff);
            // Now determine the number of common beacons
            match_count = MAX(match_count, countCommonBeacons(s1, s2));
            if ((s2->pos == NULL) && (match_count >= 12)) {
//                printf("%d: ", s2->id); printPoint(&diff); printf("\n");
                s2->pos = malloc(sizeof(point));
                s2->pos->x = diff.x;
                s2->pos->y = diff.y;
                s2->pos->z = diff.z;
                return match_count;
            } else {
                // move the scanner back
                diff = mkPoint(-diff.x, -diff.y, -diff.z);
                moveScanner(s2, diff);
            }
        }
    }

    return match_count;
}

int countMatchingBeaconsByRotation(scanner *s1, scanner *s2) {
    int match_count = 0;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int count = countMatchingBeaconsByOffset(s1, s2);
            match_count = MAX(match_count, count);
            if (match_count >= 12) return match_count;

            rotateScanner(s2, AXIS_Z);
        }
        rotateScanner(s2, AXIS_Y);
    }
    rotateScanner(s2, AXIS_X);
    for (int i = 0; i < 4; i++) {
        match_count = MAX(match_count, countMatchingBeaconsByOffset(s1, s2));
        if (match_count >= 12) return match_count;

        rotateScanner(s2, AXIS_Z);
    }
    rotateScanner(s2, AXIS_X);
    rotateScanner(s2, AXIS_X);
    for (int i = 0; i < 4; i++) {
        match_count = MAX(match_count, countMatchingBeaconsByOffset(s1, s2));
        if (match_count >= 12) return match_count;

        rotateScanner(s2, AXIS_Z);
    }
    rotateScanner(s2, AXIS_X);

    return match_count;
}

int distanceManhattan(point *p1, point *p2) {
    return abs(p1->x - p2->x) + abs(p1->y - p2->y) + abs(p1->z - p2->z);
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc != 1) {
        fname = argv[1];
    }

    readData(fname);

    // Fix the location of scanner 0
    scanners[0].pos = malloc(sizeof(point));
    scanners[0].pos->x = 0;
    scanners[0].pos->y = 0;
    scanners[0].pos->z = 0;
    // try matching scanners
    // - to optimize, we can introduce a stack of resolved scanners
    //   and only test those against non-resolved scanners
//    for (int i = 0; i < scanner_count; i++) {
//        for (int j = i + 1; j < scanner_count; j++) {
//            int cnt = countMatchingBeaconsByRotation(&scanners[i], &scanners[j]);
//            //int cnt = countMatchingBeaconsByOffset(&scanners[i], &scanners[j]);
//            if (cnt > 1) printf("\n%d -> %d: %d\n", i, j, cnt);
//            else { printf("."); fflush(stdout); }
//        }
//    }
//    printf("\n");
    
    int *scanner_stack = malloc(scanner_count * sizeof(int));
    for (int i = 0; i < scanner_count; i++) scanner_stack[i] = -1;
    int current_index = 0;
    scanner_stack[current_index] = 0;
    int push_index = 1;
    while (current_index < scanner_count) {
        assert(current_index < push_index);
        scanner *current_scanner = &scanners[scanner_stack[current_index]];
        for (int i = 0; i < scanner_count; i++) {
            if (scanners[i].pos == NULL) { // i.e. it is not resolved agains a known one yet
                int count = countMatchingBeaconsByRotation(current_scanner, &scanners[i]);
                if (count >= 12) printf("\n%d -> %d: %d\n", scanner_stack[current_index], i, count);
                else { printf("."); fflush(stdout); }
                if (count >= 12) {
                    scanner_stack[push_index++] = i;
                }
            }
        }
//        printf("\nCurrent index = %d, push index = %d\n", current_index, push_index);
//        for (int i = 0; i < push_index; i++) printf("%d,", scanner_stack[i]);
        current_index++;
    }

    int scanner1 = -1;
    int scanner2 = -1;
    int distance = 0;
    for (int i = 0; i < scanner_count; i++) {
        assert(scanners[i].pos != NULL);
        printf("%d: ", i); printPoint(scanners[i].pos); printf("\n");
        for (int j = i + 1; j < scanner_count; j++) {
            assert(scanners[j].pos != NULL);
            int d = distanceManhattan(scanners[i].pos, scanners[j].pos);
            if (d > distance) {
                distance = d;
                scanner1 = i; scanner2 = j;
            }
        }
    }
    printf("Largest Manhattan distance (between %d and %d): %d\n", scanner1, scanner2, distance);

    return EXIT_SUCCESS;
}


