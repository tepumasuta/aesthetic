// Copyright 2021 Alexey Kutepov <reximkut@gmail.com>

// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:

// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SV_H_
#define SV_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    size_t count;
    const char *data;
} string_view;

#define SV(cstr_lit) sv_from_parts(cstr_lit, sizeof(cstr_lit) - 1)
#define SV_STATIC(cstr_lit)   \
    {                         \
        sizeof(cstr_lit) - 1, \
        (cstr_lit)            \
    }

#define SV_NULL sv_from_parts(NULL, 0)

// printf macros for string_view
#define SV_Fmt "%.*s"
#define SV_Arg(sv) (int) (sv).count, (sv).data
// USAGE:
//   string_view name = ...;
//   printf("Name: "SV_Fmt"\n", SV_Arg(name));

string_view sv_from_parts(const char *data, size_t count);
string_view sv_from_cstr(const char *cstr);
string_view sv_trim_left(string_view sv);
string_view sv_trim_right(string_view sv);
string_view sv_trim(string_view sv);
string_view sv_take_left_while(string_view sv, bool (*predicate)(char x));
string_view sv_chop_by_delim(string_view *sv, char delim);
string_view sv_chop_by_sv(string_view *sv, string_view thicc_delim);
bool sv_try_chop_by_delim(string_view *sv, char delim, string_view *chunk);
string_view sv_chop_left(string_view *sv, size_t n);
string_view sv_chop_right(string_view *sv, size_t n);
string_view sv_chop_left_while(string_view *sv, bool (*predicate)(char x));
bool sv_index_of(string_view sv, char c, size_t *index);
bool sv_eq(string_view a, string_view b);
bool sv_eq_ignorecase(string_view a, string_view b);
bool sv_starts_with(string_view sv, string_view prefix);
bool sv_ends_with(string_view sv, string_view suffix);
uint64_t sv_to_u64(string_view sv);

#endif  // SV_H_