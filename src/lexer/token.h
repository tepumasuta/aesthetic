#include "stdbool.h"
#include "stddef.h"

#include "sv/sv.h"

#ifndef TOKEN_H
#define TOKEN_H

enum token_type {
    Operator=0,
    Value=1,
    Keyword=2,
    Symbol=3,
    EndOfFile=4,
    Punctuation=5,
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
    PlainCopy=5,
    DefineBinding=6,
    BoostyBinding=7,
    CommentLine=8,
    Pipe=9,
    OPERATION_TYPE_SIZE
};

enum keyword_type {
    If=0,
    When=1,
    Whenever=2,
    Exist=3,
    On=4,
    KEYWORD_TYPE_SIZE
};

enum punctuation_type {
    Semicolon=0,
    ScopeOpen=1,
    ScopeClose=2,
    ParenthesesOpen=3,
    ParenthesesClose=4,
    BracketOpen=5,
    BracketClose=6,
    Comma=7,
    Dot=8,
    LineEnd=9,
    Colon=10,
    PUNCTUATION_TYPE_SIZE
};

typedef struct operator_token {
    enum operation_type op_type;
    size_t length;
} operator_token;

typedef struct integer_literal_token {
    enum integer_literal_type int_lit_type;
} integer_literal_token;

typedef struct value_token {
    string_view contents;
    enum value_type val_type;
    union {
        integer_literal_token int_lit;
    };
} value_token;

typedef struct keyword_token {
    enum keyword_type kw_type;
} keyword_token;

typedef struct symbol_token {
    string_view contents;
} symbol_token;

typedef struct punctuation_token {
    enum punctuation_type pt_type;
} punctuation_token;

typedef struct position {
    size_t line, col;
} position;

typedef struct token {
    bool valid;
    enum token_type type;
    position pos;
    union {
        operator_token op;
        value_token val;
        keyword_token kw;
        symbol_token sym;
        punctuation_token pt;
    };
} token;

void print_token(token t);

#endif // TOKEN_H
