#include "stddef.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"
#include "assert.h"
#include "token.h"
#include "lexer.h"
#include "sv/sv.h"

typedef struct LEXER_impl {
    char *start;
    string_view pos;
} LEXER;

static bool is_digit(char c);
static bool is_hex(char c);
static bool is_oct(char c);
static bool is_bin(char c);
static bool is_start_symbolic(char c);
static bool is_symbolic(char c);
static enum operation_type is_operator(string_view sv, size_t *length);
static enum keyword_type is_keyword(string_view sv, size_t *length);
static bool is_symbol(string_view sv, size_t *length);

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
    t->op.length = size;
    lexer->pos.count -= size;
    lexer->pos.data += size;
}

void convert_to_keyword(LEXER *lexer, token *t, enum keyword_type kw_type, size_t size) {
    t->valid = true;
    t->type = Keyword;
    t->kw.kw_type = kw_type;
    lexer->pos.count -= size;
    lexer->pos.data += size;
}

void convert_to_symbol(LEXER *lexer, token *t, size_t size) {
    t->valid = true;
    t->type = Symbol;
    t->sym.contents = sv_from_parts(lexer->pos.data, size);
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

    size_t size;

    enum operation_type op_type;
    if ((op_type = is_operator(lexer->pos, &size)) != OPERATION_TYPE_SIZE) {
        convert_to_operator(lexer, &t, op_type, size);
        return t;
    }

    enum keyword_type kw_type;
    if ((kw_type = is_keyword(lexer->pos, &size)) != KEYWORD_TYPE_SIZE) {
        convert_to_keyword(lexer, &t, kw_type, size);
        return t;
    }

    if (is_symbol(lexer->pos, &size)) {
        convert_to_symbol(lexer, &t, size);
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


static bool is_digit(char c) {
    return '0' <= c && c <= '9';
}

static bool is_hex(char c) {
    return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f');
}

static bool is_oct(char c) {
    return '0' <= c && c <= '7';
}

static bool is_bin(char c) {
    return c == '0' || c == '1';
}

static bool is_start_symbolic(char c) {
    return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || c == '_';
}

static bool is_symbolic(char c) {
    return is_start_symbolic(c) || is_digit(c);
}

enum operation_type is_operator(string_view sv, size_t *length) {
    static_assert(OPERATION_TYPE_SIZE == 8, "Not all operation_type values were handled");

    // Triple symbol operators
    *length = 3;
    if (sv_starts_with(sv, SV("::="))) { return DefineBinding; }

    // Double symbol operators
    *length = 2;
    if (sv_starts_with(sv, SV("//"))) { return IntegerDivision; }
    if (sv_starts_with(sv, SV(":="))) { return BoostyBinding; }

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

static enum keyword_type is_keyword(string_view sv, size_t *length) {
    static_assert(KEYWORD_TYPE_SIZE == 1, "Not all keyword_type values were handled");

    if (sv_starts_with(sv, SV("if")) && (sv.count < 3 || !is_symbolic(sv.data[2]))) {
        *length = 2;
        return If;
    }

    *length = 0;
    return KEYWORD_TYPE_SIZE;
}

static bool is_symbol(string_view sv, size_t *length) {
    *length = 0;

    if (is_start_symbolic(*sv.data)) {
        while (sv.count-- && is_symbolic(*sv.data++)) {
            (*length)++;
        }
        return true;
    }

    return false;
}
