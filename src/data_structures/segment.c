#include "stddef.h"
#include "stdbool.h"
#include "stdio.h"

#include "segment.h"

segment segment_create(size_t start, size_t end) {
    if (start > end) end = start;
    segment result = { .start=start, .end=end };
    return result;
}

bool segment_intersect(segment seg1, segment seg2) {
    return (seg1.start <= seg2.end && seg1.end >= seg2.end) ||
           (seg2.start <= seg1.end && seg2.end >= seg1.end);
}

// O(N)
size_t segment_unite(segment *segs, size_t length, segment seg) {
    size_t intersection_start, intersection_end = length - 1;
    bool intersected = false;
    size_t insert_pos = (length ? length : 0);
    bool insert_pos_found = false;

    for (size_t i = 0; i < length; i++) {
        if (segment_intersect(segs[i], seg) && !intersected) {
            intersection_start = i;
            intersected = true;
        } else if (intersected) {
            intersection_end = i - 1;
            break;
        }

        if (!insert_pos_found && seg.end < segs[i].start) {
            insert_pos = i;
            insert_pos_found = true;
        }
    }

    if (intersected) {
        segment new_seg;
        new_seg.start = seg.start < segs[intersection_start].start
            ? seg.start
            : segs[intersection_start].start;
        new_seg.end = seg.end > segs[intersection_start].end
            ? seg.end
            : segs[intersection_start].end;
        
        segs[intersection_start] = new_seg;
        for (size_t i = intersection_start + 1, j = intersection_end + 1; j < length; i++, j++)
            segs[i] = segs[j];
        return length - intersection_end + intersection_start;
    }

    for (size_t i = length; i > insert_pos; i--)
        segs[i] = segs[i - 1];
    segs[insert_pos] = seg;

    return length + 1;
}

void print_segments(segment *segs, size_t length) {
    for (size_t i = 0; i < length - 1; i++)
        printf("(%ld, %ld) || ", segs[i].start, segs[i].end);
    printf("(%ld, %ld)\n", segs[length - 1].start, segs[length - 1].end);
}

void print_segment(segment seg) {
    printf("(%ld, %ld)\n", seg.start, seg.end);
}
