#include "string.h"
#include "stdbool.h"

#include "string_view.h"
#include "utility/character.h"


string_view sv_from_parts(char *raw, size_t length) {
    string_view result = { .start = raw, .length = length };
    return result;
}

string_view sv_from_cstr(char *str) {
    size_t length = strlen(str);
    string_view result = { .start = str, .length = length };
    return result;
}

void sv_step(string_view *sv, size_t count) {
    sv->length -= count;
    sv->start += count;
}

void sv_trim_left(string_view *sv) {
    while (is_space(*sv->start))
        sv_step(sv, 1);
}

bool sv_starts_with(string_view *sv, string_view prefix) {
    if (prefix.length > sv->length) return false;
    
    for (size_t i = 0; i < prefix.length; i++)
        if (prefix.start[i] != sv->start[i])
            return false;
    
    return true;
}
