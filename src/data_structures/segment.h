#ifndef SEGMENT_H
#define SEGMENT_H

#include "stddef.h"
#include "stdbool.h"

typedef struct {
    size_t start, end;
} segment;

segment segment_create(size_t start, size_t end);
bool segment_intersect(segment seg1, segment seg2);
size_t segment_unite(segment *segs, size_t length, segment seg);
void print_segments(segment *segs, size_t length);
void print_segment(segment seg);

#endif // SEGMENT_H
