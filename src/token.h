#include "stdbool.h"
#include "sv/sv.h"

#ifndef TOKEN_H
#define TOKEN_H

enum token_type {
    Operator=0,
    Value=1,
    Keyword=2,
    TOKEN_TYPE_SIZE
};

enum value_type {
    Integer=0,
    FloatingPoint=1,
    VALUE_TYPE_SIZE
};

enum integer_literal_type {
    Hex=0,
    Oct=1,
    Bin=2,
    Dec=3,
    INTEGER_LITERAL_TYPE_SIZE
};
enum operation_type {
    Addition=0,
    Substraction=1,
    Multiplication=2,
    FloatDivision=3,
    IntegerDivision=4,
    OPERATION_TYPE_SIZE
};

enum keyword_type {
    If=0,
    KEYWORDS_SIZE
};

typedef struct {
    enum operation_type op_type;
} operator_token;

typedef struct {
    enum integer_literal_type int_lit_type;
} integer_literal_token;

typedef struct {
    string_view contents;
    enum value_type val_type;
    union {
        integer_literal_token int_lit;
    };
} value_token;

typedef struct {
    enum keyword_type kw_type;
} keyword_token;

typedef struct {
    bool valid;
    enum token_type type;
    union {
        operator_token op;
        value_token val;
        keyword_token kw;
    };
} token;

void print_token(token t);

#endif // TOKEN_H
