#ifndef CHARACTER_H
#define CHARACTER_H

#include "stdbool.h"

bool is_digit(char c);
bool is_hex(char c);
bool is_oct(char c);
bool is_bin(char c);
bool is_start_symbolic(char c);
bool is_symbolic(char c);
bool is_string_bound(char c);
bool is_space(char c);

#endif // CHARACTER_H