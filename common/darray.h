//
// Header file and implementation for dynamic arrays
//

// External interface provides only functions to push elements
// at the end of the array

#define darray_push(a, el) _darray_push(&(a), (&el), sizeof(el))
_darray_push(void **array, void *element, int element_size);
darray_count(void *array);
darray_free(void *array);

// dynamic array stuff
#ifdef _DARRAY_IMPLEMENTATION

// Structure to hold information about the currently allocated data
// This structure is located before the the actual data that is
// exposed to the user
typedef struct _darray_info {
    int capacity;       // the potential number of elements
    int count;          // the actual current element count
    int element_size;   // the size of one element in bytes
} _darray_info;

int _darray_capacity(void *data) {
    int result = 0;

    if (data != NULL) {
        result = ((_darray_info *)data)[-1].capacity;
    }

    return result;
}

void *resize(void *array, int capacity, int element_size) {
    _darray_info *result = NULL;

    result = malloc(sizeof(int) + count * element_size);
    result[0].capacity = capacity;
    result[0].element_size = element_size;

    // TODO: copy the old data

    return (void *)result;
}
#endif

#ifdef _DARRAY_TEST
#endif
