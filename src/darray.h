#ifndef DARRAY_H
#define DARRAY_H

#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "stddef.h"

#define DARRAY_DEFINE_PROTOTYPE(DATA_TYPE) \
typedef struct {\
    DATA_TYPE *arr;\
    size_t size;\
    size_t cap;\
    bool shrink;\
} darray_##DATA_TYPE;\
\
darray_##DATA_TYPE* darray_##DATA_TYPE##_create(size_t capacity, bool shrink);\
void darray_##DATA_TYPE##_destory(darray_##DATA_TYPE *array);\
bool darray_##DATA_TYPE##_bound_get(darray_##DATA_TYPE *array, size_t index, DATA_TYPE *value);\
bool darray_##DATA_TYPE##_add(darray_##DATA_TYPE *array, DATA_TYPE element);\
bool darray_##DATA_TYPE##_delete(darray_##DATA_TYPE *array, size_t index);\
bool darray_##DATA_TYPE##_insert(darray_##DATA_TYPE *array, size_t index, DATA_TYPE element);\
bool darray_##DATA_TYPE##_reserve(darray_##DATA_TYPE *array, size_t capacity);\
void print_darray_##DATA_TYPE(darray_##DATA_TYPE *array);

#define DARRAY_DEFINE(DATA_TYPE) \
darray_##DATA_TYPE* darray_##DATA_TYPE##_create(size_t capacity, bool shrink) {\
    darray_##DATA_TYPE *array = malloc(sizeof(darray_##DATA_TYPE));\
\
    if (!array) {\
        return NULL;\
    }\
\
    array->arr = malloc(sizeof(DATA_TYPE) * capacity);\
\
    if (!array->arr) {\
        free(array);\
        return NULL;\
    }\
\
    array->cap = capacity;\
    array->size = 0;\
    array->shrink = shrink;\
\
    return array;\
}\
\
void darray_##DATA_TYPE##_destory(darray_##DATA_TYPE *array) {\
    free(array->arr);\
    free(array);\
}\
\
bool darray_##DATA_TYPE##_bound_get(darray_##DATA_TYPE *array, size_t index, DATA_TYPE *value) {\
    if (index >= array->size) {\
        return false;\
    }\
\
    *value = array->arr[index];\
\
    return true;\
}\
\
bool darray_##DATA_TYPE##_add(darray_##DATA_TYPE *array, DATA_TYPE element) {\
    if (array->cap == array->size) {\
        void *new_array = realloc(array->arr, array->cap * sizeof(DATA_TYPE) * 2);\
\
        if (!new_array) return false;\
\
        array->cap *= 2;\
        array->arr = new_array;\
    }\
\
    array->arr[array->size++] = element;\
\
    return true;\
}\
\
bool darray_##DATA_TYPE##_insert(darray_##DATA_TYPE *array, size_t index, DATA_TYPE element) {\
    if (index > array->size) return false;\
    \
    if (array->cap == array->size) {\
        void *new_array = realloc(array->arr, array->cap * sizeof(DATA_TYPE) * 2);\
\
        if (!new_array) return false;\
\
        array->cap *= 2;\
        array->arr = new_array;\
    }\
\
    for (size_t i = array->size; i > index; i++) {\
        array->arr[i] = array->arr[i - 1];\
    }\
\
    array->arr[index] = element;\
    array->size++;\
\
    return true;\
}\
\
bool darray_##DATA_TYPE##_delete(darray_##DATA_TYPE *array, size_t index) {\
    if (index >= array->size) return false;\
\
    for (size_t i = index; i + 1 < array->size; i++)\
        array->arr[i] = array->arr[i + 1];\
    array->size--;\
\
    if (array->shrink && array->size <= array->cap / 2) {\
        void *new_array = realloc(array->arr, array->cap / 2 * sizeof(DATA_TYPE));\
\
        if (!new_array) return false;\
\
        array->cap /= 2;\
        array->arr = new_array;\
    }\
\
    return true;\
}\
\
bool darray_##DATA_TYPE##_reserve(darray_##DATA_TYPE *array, size_t capacity) {\
    if (array->cap >= capacity) return false;\
\
    void *new_array = realloc(array->arr, capacity * sizeof(DATA_TYPE));\
\
    if (!new_array) {\
        return false;\
    }\
\
    array->cap = capacity;\
    array->arr = new_array;\
\
    return true;\
}

#ifdef AE_DEBUG
#define DARRAY_DEFINE_PRINT(DATA_TYPE) \
void print_darray_##DATA_TYPE(darray_##DATA_TYPE *array) {\
    printf("Darray_""DATA_TYPE""[%ld, %ld]:\n", array->size, array->cap);\
    for (size_t i = 0; i < array->size; i++) {\
        fputs("- ", stdout);\
        print_##DATA_TYPE(array->arr[i]);\
    }\
}
#else
#define DARRAY_DEFINE_PRINT(DATA_TYPE)\
void print_darray_##DATA_TYPE(darray_##DATA_TYPE *array) {\
    (void)array;\
}
#endif // AE_DEBUG

#endif // DARRAY_H