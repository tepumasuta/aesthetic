#ifndef DARRAY_H
#define DARRAY_H

#include "stdbool.h"
#include "stddef.h"

#include "token.h"

typedef struct {
    token *arr;
    size_t size;
    size_t cap;
    bool shrink;
} darray;

darray* darray_create(size_t capacity, bool shrink);
void darray_destory(darray *array);
bool darray_bound_get(darray *array, size_t index, token *value);
bool darray_add(darray *array, token element);
bool darray_delete(darray *array, size_t index);
bool darray_insert(darray *array, size_t index, token element);
bool darray_reserve(darray *array, size_t capacity);
void print_darray(darray *array);

#endif