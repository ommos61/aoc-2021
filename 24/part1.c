
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

// Global data information
int debug = 1; // 0 = no debug output, 1 = some extra debug output
#define OPCODE_NOP 0
#define OPCODE_INP 1
#define OPCODE_ADD 2
#define OPCODE_MUL 3
#define OPCODE_DIV 4
#define OPCODE_MOD 5
#define OPCODE_EQL 6
#define SRC_VALUE  -1
typedef struct instruction {
    int opcode; // OPCODE_*
    int dest;   // w = 0, x = 1, y = 2, z = 3
    int src;    // w = 0, x = 1, y = 2, z = 3, SRC_VALUE -> value
    int value;  // immediate value
} instruction;
#define INSTRUCTION_MAX 500
instruction program[INSTRUCTION_MAX];
int instructions_count = 0;

typedef struct input {
    const char *data;
    int position;
} input;

int getRegisterOrValue(const char *line, int *value) {
    const char *p = line;
    int result = SRC_VALUE;

    // skip spaces
    while (isspace(*p)) p++;
    if ((*p >= 'w') && (*p <= 'z')) {
        result = *p -'w';
    } else {
        assert(value != NULL);
        *value = atoi(p);
    }
    return result;
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

        int value = 0;
        program[instructions_count].opcode = OPCODE_NOP;
        if (strncmp(line, "inp", 3) == 0) {
            program[instructions_count].opcode = OPCODE_INP;
            program[instructions_count].dest = getRegisterOrValue(line + 3, NULL);
        } else if (strncmp(line, "add", 3) == 0) {
            program[instructions_count].opcode = OPCODE_ADD;
            program[instructions_count].dest = getRegisterOrValue(line + 3, NULL);
            program[instructions_count].src = getRegisterOrValue(line + 5, &value);
            if (program[instructions_count].src == SRC_VALUE) {
                program[instructions_count].value = value;
            }
        } else if (strncmp(line, "mul", 3) == 0) {
            program[instructions_count].opcode = OPCODE_MUL;
            program[instructions_count].dest = getRegisterOrValue(line + 3, NULL);
            program[instructions_count].src = getRegisterOrValue(line + 5, &value);
            if (program[instructions_count].src == SRC_VALUE) {
                program[instructions_count].value = value;
            }
        } else if (strncmp(line, "div", 3) == 0) {
            program[instructions_count].opcode = OPCODE_DIV;
            program[instructions_count].dest = getRegisterOrValue(line + 3, NULL);
            program[instructions_count].src = getRegisterOrValue(line + 5, &value);
            if (program[instructions_count].src == SRC_VALUE) {
                program[instructions_count].value = value;
            }
        } else if (strncmp(line, "mod", 3) == 0) {
            program[instructions_count].opcode = OPCODE_MOD;
            program[instructions_count].dest = getRegisterOrValue(line + 3, NULL);
            program[instructions_count].src = getRegisterOrValue(line + 5, &value);
            if (program[instructions_count].src == SRC_VALUE) {
                program[instructions_count].value = value;
            }
        } else if (strncmp(line, "eql", 3) == 0) {
            program[instructions_count].opcode = OPCODE_EQL;
            program[instructions_count].dest = getRegisterOrValue(line + 3, NULL);
            program[instructions_count].src = getRegisterOrValue(line + 5, &value);
            if (program[instructions_count].src == SRC_VALUE) {
                program[instructions_count].value = value;
            }
        } else {
            assert(0 && "Unknown instruction");
        }
        instructions_count++;
    }

    printf("instruction = %d\n", instructions_count);
    fclose(fin);
}

void printRegisters(long rs[4]) {
    for (int i = 0; i < 4; i++) {
        printf("%c = %ld ", 'w' + i, rs[i]);
    }
    printf("\n");
}

void print_instruction(instruction *program, int pc) {
    instruction *inst = program + pc;
    if (pc < 0) return;
    switch (inst->opcode) {
    case OPCODE_INP:
        printf("inp %c", 'w' + inst->dest);
        break;
    case OPCODE_ADD:
        printf("add %c, ", 'w' + inst->dest);
        if (inst->src == SRC_VALUE) printf("#%d", inst->value); else printf("%c", 'w' + inst->src);
        break;
    case OPCODE_MUL:
        printf("mul %c, ", 'w' + inst->dest);
        if (inst->src == SRC_VALUE) printf("#%d", inst->value); else printf("%c", 'w' + inst->src);
        break;
    case OPCODE_DIV:
        printf("div %c, ", 'w' + inst->dest);
        if (inst->src == SRC_VALUE) printf("#%d", inst->value); else printf("%c", 'w' + inst->src);
        break;
    case OPCODE_MOD:
        printf("mod %c, ", 'w' + inst->dest);
        if (inst->src == SRC_VALUE) printf("#%d", inst->value); else printf("%c", 'w' + inst->src);
        break;
    case OPCODE_EQL:
        printf("eql %c, ", 'w' + inst->dest);
        if (inst->src == SRC_VALUE) printf("#%d", inst->value); else printf("%c", 'w' + inst->src);
        break;
    }
}

char get_input_char(struct input *input_data) {
    assert(input_data->data[input_data->position] != '\000');
    char result = input_data->data[input_data->position];
    input_data->position += 1;

    return result;
}

void execute_instruction(int pc, long *registers, struct input *input_data) {
    long value = 0;

    switch (program[pc].opcode) {
    case OPCODE_NOP:
        printf("NOP should not occur in a program\n");
        break;
    case OPCODE_INP:
        registers[program[pc].dest] = get_input_char(input_data) - '0';
        break;
    case OPCODE_ADD:
        if (program[pc].src == SRC_VALUE) {
            value = program[pc].value;
        } else {
            value = registers[program[pc].src];
        }
        registers[program[pc].dest] += value;
        break;
    case OPCODE_MUL:
        if (program[pc].src == SRC_VALUE) {
            value = program[pc].value;
        } else {
            value = registers[program[pc].src];
        }
        registers[program[pc].dest] *= value;
        break;
    case OPCODE_DIV:
        if (program[pc].src == SRC_VALUE) {
            value = program[pc].value;
        } else {
            value = registers[program[pc].src];
        }
        registers[program[pc].dest] /= value;
        break;
    case OPCODE_MOD:
        if (program[pc].src == SRC_VALUE) {
            value = program[pc].value;
        } else {
            value = registers[program[pc].src];
        }
        registers[program[pc].dest] %= value;
        break;
    case OPCODE_EQL:
        if (program[pc].src == SRC_VALUE) {
            value = program[pc].value;
        } else {
            value = registers[program[pc].src];
        }
        registers[program[pc].dest] = (registers[program[pc].dest] == value) ? 1 : 0;
        break;
    default:
        printf("Error: unhandled opcode '%d'\n", program[pc].opcode);
        break;
    }
}

long run(const char *input) {
    long registers[4] = {0};
    struct input input_data;
    input_data.data = input;
    input_data.position = 0;

    if (debug) printf("Input: %s\n", input);
    for (int i = 0; i < instructions_count; i++) {
        execute_instruction(i, registers, &input_data);
    }
    if (debug) printRegisters(registers);

    return registers[3];
}

#define BLOCK 18
void determine_inputs(void) {
    int stack_pointer = 0;
    struct stack_element {
        int index;
        int value;
    } stack[10];
    char input_max[20] = { 0 };
    char input_min[20] = { 0 };

    printf("zd  cx  cy\n");
    for (int i = 0; i < 14; i++) {
        assert(program[i * BLOCK].opcode == OPCODE_INP);
        int zd = program[i * BLOCK + 4].value;
        int cx = program[i * BLOCK + 5].value;
        int cy = program[i * BLOCK + 15].value;
        printf("%-4d%-4d%-4d\n", zd, cx, cy);
        if (zd == 1) {
            // push onto stack
            stack[stack_pointer].index = i;
            stack[stack_pointer].value = cy;
            stack_pointer += 1;
        } else if (zd == 26) {
            // pop off the stack and resolve the values
            stack_pointer -= 1;
            int index = stack[stack_pointer].index;
            int value = stack[stack_pointer].value + cx;
            if (value < 0) {
                input_max[index] = '9';
                input_max[i] = '0' + 9 + value;
                input_min[i] = '1';
                input_min[index] = '0' + 1 - value;
            } else {
                input_max[i] = '9';
                input_max[index] = '0' + 9 - value;
                input_min[index] = '1';
                input_min[i] = '0' + 1 + value;
            }
        } else {
            printf("zd should always be 1 or 26.\n");
        }
    }
    printf("\n");
    printf("Max input should be '%s'.\n", input_max);
    long z = run(input_max);
    printf("z = %ld\n", z);
    printf("Min input should be '%s'.\n", input_min);
    z = run(input_min);
    printf("z = %ld\n", z);
}

#define CLEARSCREEN "[2J"
#define GOTOYX      "[%d;%dH"
#define CLEARTOEOL  "[K"
#define CURSOR_SHOW "[>25h"
#define CURSOR_HIDE "[>25j"

void dbg_display_registers(long *registers) {
    for (int i = 0; i < 4; i++) {
        printf(GOTOYX, i + 1, 60); printf("| %c: %ld" CLEARTOEOL, 'w' + i, registers[i]);
    }
    printf(GOTOYX, 5, 60); printf(CLEARTOEOL "---------------------");
}

void dbg_display_input(input *input_data) {
    printf(GOTOYX, 6, 60);
    printf("| %.*s>%s " CLEARTOEOL, input_data->position, input_data->data, input_data->data + input_data->position);
    printf(GOTOYX, 7, 60); printf(CLEARTOEOL "---------------------");
}

void dbg_display_source(int pc) {
    for (int i = 0; i < 20; i++) {
        printf(GOTOYX, i + 2, 5); printf("                    ");
        printf(GOTOYX, i + 2, 5); print_instruction(program, pc + i - 2);
        if (i == 2) { printf(GOTOYX, i + 2, 1); printf("-->"); }
    }
}

void debugger(char *input) {
    int quit = 0;
    char last_command[20] = { 0 };
    long registers[4] = { 0 };
    int pc = 0;
    struct input input_data;
    input_data.data = input;
    input_data.position = 0;

    printf(CLEARSCREEN);
    while (! quit) {
        dbg_display_registers(registers);
        dbg_display_input(&input_data);
        dbg_display_source(pc);

        // get input
        printf(GOTOYX, 25, 1); printf("> ");
        char input[20];
        fscanf(stdin, "%s", input);
        if (strlen(input) == 0) {
            strcpy(input, last_command);
        }
        if ((strcmp(input, "step") == 0) || (strcmp(input, "s") == 0)) {
            strcpy(last_command, input);
            printf(GOTOYX, 26, 1); printf("step instruction");
            execute_instruction(pc, registers, &input_data);
            pc += 1;
        } else if ((strcmp(input, "run") == 0) || (strcmp(input, "r") == 0)) {
            strcpy(last_command, input);
            printf(GOTOYX, 26, 1); printf("run instruction");
            while (program[pc].opcode != OPCODE_INP) {
                execute_instruction(pc, registers, &input_data);
                pc += 1;
            }
        } else if ((strcmp(input, "quit") == 0) || (strcmp(input, "q") == 0)) {
            strcpy(last_command, input);
            printf(GOTOYX, 27, 1);
            quit = 1;
            continue;
        } else {
            printf(GOTOYX "Commands are: run/step/quit" CLEARTOEOL, 26, 1);
        }
        printf(GOTOYX CLEARTOEOL, 25, 3);
    }
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc != 1) {
        fname = argv[1];
    }

    readData(fname);

    // some test runs
    if (debug) {
        long z = 0;
        z = run("11111111111111");
        printf("z = %ld\n", z);
        z = run("99999999999999");
        printf("z = %ld\n", z);
    }

    // determine the max and min input strings
    determine_inputs();

    // start the debugger
    printf("Press <enter> to continue to the debugger.\n");
    fgetc(stdin);
    debugger("89913949293939");

    return EXIT_SUCCESS;
}

