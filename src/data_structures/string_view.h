#ifndef STRING_VIEW_H
#define STRING_VIEW_H

#include "stdbool.h"
#include "stddef.h"


typedef struct string_view {
    char *start;
    size_t length;
} string_view;

#define SV(cstr_lit) sv_from_parts(cstr_lit, sizeof(cstr_lit) - 1)
#define SV_Fmt "%.*s"
#define SV_Arg(sv) (int) (sv).length, (sv).start

string_view sv_from_parts(char *raw, size_t length);
string_view sv_from_cstr(char *str);
void sv_step(string_view *sv, size_t count);
void sv_trim_left(string_view *sv);
bool sv_starts_with(string_view *sv, string_view prefix);

#endif // STRING_VIEW_H
