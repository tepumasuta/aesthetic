#include "sv.h"

string_view sv_from_parts(const char *data, size_t count)
{
    string_view sv;
    sv.count = count;
    sv.data = data;
    return sv;
}

string_view sv_from_cstr(const char *cstr)
{
    return sv_from_parts(cstr, strlen(cstr));
}

string_view sv_trim_left(string_view sv)
{
    size_t i = 0;
    while (i < sv.count && isspace(sv.data[i])) {
        i += 1;
    }

    return sv_from_parts(sv.data + i, sv.count - i);
}

string_view sv_trim_right(string_view sv)
{
    size_t i = 0;
    while (i < sv.count && isspace(sv.data[sv.count - 1 - i])) {
        i += 1;
    }

    return sv_from_parts(sv.data, sv.count - i);
}

string_view sv_trim(string_view sv)
{
    return sv_trim_right(sv_trim_left(sv));
}

string_view sv_chop_left(string_view *sv, size_t n)
{
    if (n > sv->count) {
        n = sv->count;
    }

    string_view result = sv_from_parts(sv->data, n);

    sv->data  += n;
    sv->count -= n;

    return result;
}

string_view sv_chop_right(string_view *sv, size_t n)
{
    if (n > sv->count) {
        n = sv->count;
    }

    string_view result = sv_from_parts(sv->data + sv->count - n, n);

    sv->count -= n;

    return result;
}

bool sv_index_of(string_view sv, char c, size_t *index)
{
    size_t i = 0;
    while (i < sv.count && sv.data[i] != c) {
        i += 1;
    }

    if (i < sv.count) {
        if (index) {
            *index = i;
        }
        return true;
    } else {
        return false;
    }
}

bool sv_try_chop_by_delim(string_view *sv, char delim, string_view *chunk)
{
    size_t i = 0;
    while (i < sv->count && sv->data[i] != delim) {
        i += 1;
    }

    string_view result = sv_from_parts(sv->data, i);

    if (i < sv->count) {
        sv->count -= i + 1;
        sv->data  += i + 1;
        if (chunk) {
            *chunk = result;
        }
        return true;
    }

    return false;
}

string_view sv_chop_by_delim(string_view *sv, char delim)
{
    size_t i = 0;
    while (i < sv->count && sv->data[i] != delim) {
        i += 1;
    }

    string_view result = sv_from_parts(sv->data, i);

    if (i < sv->count) {
        sv->count -= i + 1;
        sv->data  += i + 1;
    } else {
        sv->count -= i;
        sv->data  += i;
    }

    return result;
}

string_view sv_chop_by_sv(string_view *sv, string_view thicc_delim)
{
    string_view window = sv_from_parts(sv->data, thicc_delim.count);
    size_t i = 0;
    while (i + thicc_delim.count < sv->count 
        && !(sv_eq(window, thicc_delim))) 
    {
        i++;
        window.data++;
    }

    string_view result = sv_from_parts(sv->data, i);

    if (i + thicc_delim.count == sv->count) {
        // include last <thicc_delim.count> characters if they aren't 
        //  equal to thicc_delim
        result.count += thicc_delim.count; 
    }
    
    // Chop!
    sv->data  += i + thicc_delim.count;
    sv->count -= i + thicc_delim.count;

    return result;
}

bool sv_starts_with(string_view sv, string_view expected_prefix)
{
    if (expected_prefix.count <= sv.count) {
        string_view actual_prefix = sv_from_parts(sv.data, expected_prefix.count);
        return sv_eq(expected_prefix, actual_prefix);
    }

    return false;
}

bool sv_ends_with(string_view sv, string_view expected_suffix)
{
    if (expected_suffix.count <= sv.count) {
        string_view actual_suffix = sv_from_parts(sv.data + sv.count - expected_suffix.count, expected_suffix.count);
        return sv_eq(expected_suffix, actual_suffix);
    }

    return false;
}

bool sv_eq(string_view a, string_view b)
{
    if (a.count != b.count) {
        return false;
    } else {
        return memcmp(a.data, b.data, a.count) == 0;
    }
}

bool sv_eq_ignorecase(string_view a, string_view b)
{
    if (a.count != b.count) {
        return false;
    }
    
    char x, y;
    for (size_t i = 0; i < a.count; i++) {
        x = 'A' <= a.data[i] && a.data[i] <= 'Z'
              ? a.data[i] + 32
              : a.data[i];
        
        y = 'A' <= b.data[i] && b.data[i] <= 'Z'
              ? b.data[i] + 32
              : b.data[i];

        if (x != y) return false;
    } 
    return true;
}

uint64_t sv_to_u64(string_view sv)
{
    uint64_t result = 0;

    for (size_t i = 0; i < sv.count && isdigit(sv.data[i]); ++i) {
        result = result * 10 + (uint64_t) sv.data[i] - '0';
    }

    return result;
}

string_view sv_chop_left_while(string_view *sv, bool (*predicate)(char x))
{
    size_t i = 0;
    while (i < sv->count && predicate(sv->data[i])) {
        i += 1;
    }
    return sv_chop_left(sv, i);
}

string_view sv_take_left_while(string_view sv, bool (*predicate)(char x))
{
    size_t i = 0;
    while (i < sv.count && predicate(sv.data[i])) {
        i += 1;
    }
    return sv_from_parts(sv.data, i);
}