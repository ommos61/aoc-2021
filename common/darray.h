// dynamic array stuff
typedef struct darray_info {
    int count;
    int element_size;
} darray_info;

int capacity(void *data) {
    int result = 0;

    if (data != NULL) {
        result = ((darray_info *)data)[-1].count;
    }

    return result;
}

void *resize(void *data, int count, int element_size) {
    darray_info *result = NULL;

    result = malloc(sizeof(int) + count * element_size);
    result[0].count = count;
    result[0].element_size = element_size;

    // TODO: copy the old data

    return (void *)result;
}

