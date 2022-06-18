#include "stddef.h"
#include "stdbool.h"
#include "stdlib.h"
#include "stdio.h"

#include "token.h"
#include "darray.h"

darray* darray_create(size_t capacity, bool shrink) {
    darray *array = malloc(sizeof(darray));

    if (!array) {
        return NULL;
    }

    array->arr = malloc(sizeof(token) * capacity);

    if (!array->arr) {
        free(array);
        return NULL;
    }

    array->cap = capacity;
    array->size = 0;
    array->shrink = shrink;

    return array;
}

void darray_destory(darray *array) {
    free(array->arr);
    free(array);
}

bool darray_bound_get(darray *array, size_t index, token *value) {
    if (index >= array->size) {
        return false;
    }

    *value = array->arr[index];

    return true;
}

bool darray_add(darray *array, token element) {
    if (array->cap == array->size) {
        token *new_array = realloc(array->arr, array->cap * sizeof(token) * 2);

        if (!new_array) return false;

        array->cap *= 2;
        array->arr = new_array;
    }

    array->arr[array->size++] = element;

    return true;
}

bool darray_insert(darray *array, size_t index, token element) {
    if (index > array->size) return false;
    
    if (array->cap == array->size) {
        token *new_array = realloc(array->arr, array->cap * sizeof(token) * 2);

        if (!new_array) return false;

        array->cap *= 2;
        array->arr = new_array;
    }

    for (size_t i = array->size; i > index; i++) {
        array->arr[i] = array->arr[i - 1];
    }

    array->arr[index] = element;
    array->size++;

    return true;
}

bool darray_delete(darray *array, size_t index) {
    if (index >= array->size) return false;

    for (size_t i = index; i + 1 < array->size; i++) {
        array->arr[i] = array->arr[i + 1];
    }
    array->size--;

    if (array->shrink && array->size <= array->cap / 2) {
        token *new_array = realloc(array->arr, array->cap / 2 * sizeof(token));

        if (!new_array) return false;

        array->cap /= 2;
        array->arr = new_array;
    }

    return true;
}

bool darray_reserve(darray *array, size_t capacity) {
    if (array->cap >= capacity) return false;

    token *new_array = realloc(array->arr, capacity * sizeof(token));

    if (!new_array) {
        return false;
    }

    array->cap = capacity;
    array->arr = new_array;

    return true;
}

#ifdef AE_DEBUG
void print_darray(darray *array) {
    printf("Darray[%ld, %ld]:\n", array->size, array->cap);
    for (size_t i = 0; i < array->size; i++) {
        fputs("- ", stdout);
        print_token(array->arr[i]);
    }
}
#else
void print_darray(darray *array) {
    (void)array;
}
#endif
