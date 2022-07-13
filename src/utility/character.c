#include "character.h"
#include "stdbool.h"


bool is_digit(char c) { return '0' <= c && c <= '9'; }
bool is_hex(char c) { return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f'); }
bool is_oct(char c) { return '0' <= c && c <= '7'; }
bool is_bin(char c) { return c == '0' || c == '1'; }
bool is_start_symbolic(char c) { return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || c == '_'; }
bool is_symbolic(char c) { return is_start_symbolic(c) || is_digit(c); }
bool is_string_bound(char c) { return c == '"' || c == '\''; }
bool is_space(char c) { return c == '\t' || c == ' '; }
