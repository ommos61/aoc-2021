
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define array_count(a) (sizeof(a)/sizeof(a[0]))
#define LINE_LENGTH 1024

// Global data information
int debug = 1; // 0 = no debug output, 1 = some extra debug output
char *algorithm = NULL;
#define OFFSET 55
typedef struct image {
    int width;
    int height;
    int offset;
    char *data;
} image;
image input;
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

        if (algorithm == NULL) {
            algorithm = malloc(strlen(line) + 1);
            strcpy(algorithm, line);
        } else if (strlen(line) == 0) {
            /* skip empty line */
        } else {
            // image line
            if (input.data == NULL) {
                input.width = strlen(line);
                // assume it is a square
                input.data = malloc((input.width + 2 * OFFSET) * (input.width + 2 * OFFSET));
                memset(input.data, '.', (input.width + 2 * OFFSET) * (input.width + 2 * OFFSET));
                input.offset = OFFSET;
            }
            // All lines must be equal length
            assert(strlen(line) == (unsigned long)input.width);
            // There must not be more lines that the width of the image
            assert(input.height < input.width);

            int data_offset = (input.height + OFFSET) * (input.width + 2 * OFFSET) + OFFSET;
            memcpy(input.data + data_offset, line, input.width);
            input.height++;
        }
    }

    printf("Image: %dx%d\n", input.width, input.height);
    assert(input.height == input.width);
    fclose(fin);
}

void printImage(image *img) {
    for (int y = 0; y < img->height + 2 * OFFSET; y++) {
        for (int x = 0; x < img->width + 2 * OFFSET; x++) {
            printf("%c", img->data[y * (img->width + 2 * OFFSET) + x]);
        }
        printf("\n");
    }
}

char getPixel(image *img, int x, int y) {
    if (x < 0) x = 0;
    if (x >= img->width + 2 * img->offset) x = img->width + 2 * img->offset - 1;
    if (y < 0) y = 0;
    if (y >= img->height + 2 * img->offset) y = img->height + 2 * img->offset - 1;

    return img->data[y * (img->width + 2 * img->offset) + x];
}

struct algo_offsets {
    int dx; int dy;
} offsets[] = {
    { -1, -1 },
    {  0, -1 },
    {  1, -1 },
    { -1,  0 },
    {  0,  0 },
    {  1,  0 },
    { -1,  1 },
    {  0,  1 },
    {  1,  1 }
};
image applyAlgorithm(image *input, char *algorithm) {
    image output;
    output.width = input->width;
    output.height = input->height;
    output.offset = input->offset;
    output.data = malloc((output.height + 2 * OFFSET) * (output.width + 2 * OFFSET));
    memset(output.data, 'O', (output.height + 2 * OFFSET) * (output.width + 2 * OFFSET));

    for (int y = 0; y < input->height + 2 * OFFSET; y++) {
        for (int x = 0; x < input->width + 2 * OFFSET; x++) {
            int algo_index = 0;
            for (int i = 0; i < (int)array_count(offsets); i++) {
                algo_index *= 2;
                if (getPixel(input, x + offsets[i].dx, y + offsets[i].dy) == '#') {
                    algo_index += 1;
                }
            }
            output.data[y * (input->width + 2 * OFFSET) + x] = algorithm[algo_index];
        }
    }

    return output;
}

int countLitPixels(image *img) {
    int count = 0;

    for (int y = 0; y < img->height + 2 * OFFSET; y++) {
        for (int x = 0; x < img->width + 2 * OFFSET; x++) {
            if (img->data[y * (img->width + 2 * OFFSET) + x] == '#')
                count++;
        }
    }

    return count;
}

int main(int argc, char *argv[]) {
    char *fname = "input.txt";

    // when another input file is specified
    if (argc != 1) {
        fname = argv[1];
    }

    readData(fname);
//    printImage(&input);

    // implement algorithm
    image output;
    for (int i = 0; i < 50; i++) {
        output = applyAlgorithm(&input, algorithm);
//        printImage(&output); printf("\n");
        input = output;
    }
    int cnt = countLitPixels(&output);

    printImage(&output); printf("\n");
    printf("Lit pixel count: %d\n", cnt);

    return EXIT_SUCCESS;
}


