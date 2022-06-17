#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"
#include "token.h"
#include "lexer.h"
#include "sv/sv.h"

typedef struct LEXER_impl {
    char *start;
    string_view pos;
} LEXER;

bool is_digit(char c);
bool is_hex(char c);
bool is_oct(char c);
bool is_bin(char c);
enum operation_type is_operator(string_view sv, size_t *length);

void convert_to_number(LEXER *lexer, token *t) {
    const char* lp = lexer->pos.data;
    size_t count = lexer->pos.count;

    t->type = Value;
    t->valid = true;

    bool (*digit_check) (char) = is_digit;
    bool is_plain_integer = true;
    enum integer_literal_type int_type;
    
    if (sv_starts_with(lexer->pos, SV("0x"))) {
        int_type = Hex;
        lp += 2;
        count -= 2;
        digit_check = is_hex;
    } else if (sv_starts_with(lexer->pos, SV("0o"))) {
        int_type = Oct;
        lp += 2;
        count -= 2;
        digit_check = is_oct;
    } else if (sv_starts_with(lexer->pos, SV("0b"))) {
        int_type = Bin;
        lp += 2;
        count -= 2;
        digit_check = is_bin;
    } else {
        int_type = Dec;
    }

    while (count > 0 && digit_check(*lp)) {
        lp++;
        count--;
    }

    if (count && int_type == Dec && *lp == '.') {
        is_plain_integer = false;
        
        lp++;
        count--;
        
        while (count > 0 && digit_check(*lp)) {
            lp++;
            count--;
        }
    }

    size_t passed = lp - lexer->pos.data;

    t->val.contents = sv_from_parts(lexer->pos.data, passed);

    if (is_plain_integer) {
        t->val.val_type = Integer;
        t->val.int_lit.int_lit_type = int_type;
    } else {
        t->val.val_type = FloatingPoint;
    }

    sv_chop_left(&(lexer->pos), passed);
}

void convert_to_operator(LEXER *lexer, token *t, enum operation_type op_type, size_t size) {
    t->valid = true;
    t->type = Operator;
    t->op.op_type = op_type;
    lexer->pos.count -= size;
    lexer->pos.data += size;
}


token lex_token(LEXER *lexer) {
    token t;
    t.valid = false;
    if (!lexer || !lexer->pos.data[0]) {
        return t;
    }

    lexer->pos = sv_trim_left(lexer->pos);

    if (is_digit(*lexer->pos.data)) {
        convert_to_number(lexer, &t);
        return t;
    }

    enum operation_type op_type;
    size_t size;
    if ((op_type = is_operator(lexer->pos, &size)) != OPERATION_TYPE_SIZE) {
        convert_to_operator(lexer, &t, op_type, size);
        return t;
    }

    return t;
}

LEXER* lexer_from_text(const char *text) {
    LEXER* lexer = malloc(sizeof(LEXER));

    if (!lexer) {
        return NULL;
    }

    // lexer->start = strdup(text);
    size_t text_length = (strlen(text) + 1);
    lexer->start = (char *)malloc(text_length * sizeof(char));
    memcpy(lexer->start, text, text_length);

    lexer->pos = sv_from_cstr(lexer->start);

    return lexer;
}

void lexer_destroy(LEXER* lexer) {
    free(lexer->start);
    free(lexer);
}


bool is_digit(char c) {
    return '0' <= c && c <= '9';
}

bool is_hex(char c) {
    return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f');
}

bool is_oct(char c) {
    return '0' <= c && c <= '7';
}

bool is_bin(char c) {
    return c == '0' || c == '1';
}

enum operation_type is_operator(string_view sv, size_t *length) {
    // Double symbol operators
    *length = 2;
    if (sv_starts_with(sv, SV("//"))) { return IntegerDivision; }

    // Mono symbol operators
    *length = 1;
    if (sv_starts_with(sv, SV("+"))) { return Addition; }
    if (sv_starts_with(sv, SV("-"))) { return Substraction; }
    if (sv_starts_with(sv, SV("*"))) { return Multiplication; }
    if (sv_starts_with(sv, SV("/"))) { return FloatDivision; }
    if (sv_starts_with(sv, SV("="))) { return PlainCopy; }

    *length = 0;
    return OPERATION_TYPE_SIZE;
}
