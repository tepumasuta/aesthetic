#include "stddef.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"
#include "assert.h"

#include "token.h"
#include "data_structures/string_view.h"
#include "lexer.h"

typedef struct LEXER_impl {
    position current_pos;
    char *start;
    string_view text_pos;
} LEXER;

static bool is_digit(char c);
static bool is_hex(char c);
static bool is_oct(char c);
static bool is_bin(char c);
static bool is_start_symbolic(char c);
static bool is_symbolic(char c);
static bool is_string_bound(char c);
static enum operation_type is_operator(string_view sv, size_t *length);
static enum keyword_type is_keyword(string_view sv, size_t *length);
static enum punctuation_type is_punctuation(string_view sv, size_t *length);
static bool is_symbol(string_view sv, size_t *length);

static void convert_to_number(LEXER *lexer, token *t) {
    const char* lp = lexer->text_pos.start;
    size_t count = lexer->text_pos.length;

    t->type = Value;
    t->valid = true;

    bool (*digit_check) (char) = is_digit;
    bool is_plain_integer = true;
    enum integer_literal_type int_type;
    
    if (sv_starts_with(&lexer->text_pos, SV("0x"))) {
        int_type = Hex;
        lp += 2;
        count -= 2;
        digit_check = is_hex;
    } else if (sv_starts_with(&lexer->text_pos, SV("0o"))) {
        int_type = Oct;
        lp += 2;
        count -= 2;
        digit_check = is_oct;
    } else if (sv_starts_with(&lexer->text_pos, SV("0b"))) {
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

    size_t passed = lp - lexer->text_pos.start;

    t->val.contents = sv_from_parts(lexer->text_pos.start, passed);

    if (is_plain_integer) {
        t->val.val_type = Integer;
        t->val.int_lit.int_lit_type = int_type;
    } else {
        t->val.val_type = FloatingPoint;
    }

    sv_step(&lexer->text_pos, passed);
    lexer->current_pos.col += passed;
}

static void convert_to_string(LEXER *lexer, token *t) {
    assert(0 && "Not implemented yet");
}

static void convert_to_operator(LEXER *lexer, token *t, enum operation_type op_type, size_t size) {
    t->valid = true;
    t->type = Operator;
    t->op.op_type = op_type;
    t->op.length = size;
    sv_step(&lexer->text_pos, size);
    lexer->current_pos.col += size;
}

static void convert_to_keyword(LEXER *lexer, token *t, enum keyword_type kw_type, size_t size) {
    t->valid = true;
    t->type = Keyword;
    t->kw.kw_type = kw_type;
    sv_step(&lexer->text_pos, size);
    lexer->current_pos.col += size;
}

static void convert_to_symbol(LEXER *lexer, token *t, size_t size) {
    t->valid = true;
    t->type = Symbol;
    t->sym.contents = sv_from_parts(lexer->text_pos.start, size);
    sv_step(&lexer->text_pos, size);
    lexer->current_pos.col += size;
}

static void convert_to_punctuation(LEXER *lexer, token *t, enum punctuation_type pt_type, size_t size) {
    t->valid = true;
    t->type = Punctuation;
    t->pt.pt_type = pt_type;
    sv_step(&lexer->text_pos, size);
    lexer->current_pos.col += size;
}


token lex_token(LEXER *lexer) {
    token t;
    t.valid = false;

    if (!lexer) {
        // TODO: Throw an error
        return t;
    }

    t.pos = lexer->current_pos;

    if (!lexer->text_pos.start[0]) {
        t.valid = true;
        t.type = EndOfFile;
        return t;
    }

    const char *before = lexer->text_pos.start;
    sv_trim_left(&lexer->text_pos);
    size_t spaces = lexer->text_pos.start - before;
    lexer->current_pos.col += spaces;

    t.pos = lexer->current_pos;    

    if (is_digit(*lexer->text_pos.start)) {
        convert_to_number(lexer, &t);
        return t;
    }

    if (is_string_bound(*lexer->text_pos.data)) {
        convert_to_string(lexer, &t);
        return t;
    }

    size_t size;

    enum operation_type op_type;
    if ((op_type = is_operator(lexer->text_pos, &size)) != OPERATION_TYPE_SIZE) {
        convert_to_operator(lexer, &t, op_type, size);
        return t;
    }

    enum keyword_type kw_type;
    if ((kw_type = is_keyword(lexer->text_pos, &size)) != KEYWORD_TYPE_SIZE) {
        convert_to_keyword(lexer, &t, kw_type, size);
        return t;
    }

    enum punctuation_type pt_type;
    if ((pt_type = is_punctuation(lexer->text_pos, &size)) != PUNCTUATION_TYPE_SIZE) {
        convert_to_punctuation(lexer, &t, pt_type, size);
       
        if (pt_type == LineEnd) {
            lexer->current_pos.line++;
            lexer->current_pos.col = 0;
        }
            
        return t;
    }

    if (is_symbol(lexer->text_pos, &size)) {
        convert_to_symbol(lexer, &t, size);
        return t;
    }

    return t;
}

LEXER *lexer_from_text(char *text) {
    LEXER *lexer = malloc(sizeof(LEXER));

    if (!lexer) {
        return NULL;
    }

    // *content_holder = strdup(text);
    size_t text_length = (strlen(text) + 1);
    lexer->start = malloc(text_length * sizeof(char));
    memcpy(lexer->start, text, text_length);
    lexer->text_pos = sv_from_cstr(lexer->start);
    lexer->current_pos.line = 1;
    lexer->current_pos.col = 1;

    return lexer;
}

void lexer_destroy(LEXER* lexer) {
    free(lexer->start);
    free(lexer);
}


static bool is_digit(char c) { return '0' <= c && c <= '9'; }
static bool is_hex(char c) { return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f'); }
static bool is_oct(char c) { return '0' <= c && c <= '7'; }
static bool is_bin(char c) { return c == '0' || c == '1'; }
static bool is_start_symbolic(char c) { return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || c == '_'; }
static bool is_symbolic(char c) { return is_start_symbolic(c) || is_digit(c); }

static bool compare_full_keyword(string_view sv, char *word, size_t *length) {
    size_t count = strlen(word);
    
    if (sv_starts_with(&sv, sv_from_cstr(word))
    && (sv.length < count + 1 || !is_symbolic(sv.start[count]))) {
        *length = count;
        return true;
    }
    return false;
}

static bool compare_prefix(string_view sv, char *word, size_t *length) {
    size_t count = strlen(word);

    if (sv_starts_with(&sv, sv_from_cstr(word))) {
        *length = count;
        return true;
    }

    return false;
}

static bool is_string_bound(char c) {
    return c == '"' || c == '\'';
}

static enum operation_type is_operator(string_view sv, size_t *length) {
    static_assert(OPERATION_TYPE_SIZE == 12, "Not all operation_type values were handled");

    if (compare_prefix(sv, "::=", length)) { return DefineBinding; }
    if (compare_prefix(sv, "///", length)) { return CommentLine; }
    if (compare_prefix(sv, "~!", length)) { return RecursiveDelete; }
    if (compare_prefix(sv, "!!", length)) { return Delete; }
    if (compare_prefix(sv, "~>", length)) { return Pipe; }
    if (compare_prefix(sv, "//", length)) { return IntegerDivision; }
    if (compare_prefix(sv, ":=", length)) { return BoostyBinding; }
    if (compare_prefix(sv, "+", length)) { return Addition; }
    if (compare_prefix(sv, "-", length)) { return Substraction; }
    if (compare_prefix(sv, "*", length)) { return Multiplication; }
    if (compare_prefix(sv, "/", length)) { return FloatDivision; }
    if (compare_prefix(sv, "=", length)) { return PlainCopy; }

    *length = 0;
    return OPERATION_TYPE_SIZE;
}

static enum keyword_type is_keyword(string_view sv, size_t *length) {
    static_assert(KEYWORD_TYPE_SIZE == 5, "Not all keyword_type values were handled");

    if (compare_full_keyword(sv, "whenever", length)) { return Whenever; }
    if (compare_full_keyword(sv, "exist", length)) { return Exist; }
    if (compare_full_keyword(sv, "when", length)) { return When; }
    if (compare_full_keyword(sv, "on", length)) { return On; }
    if (compare_full_keyword(sv, "if", length)) { return If; }

    *length = 0;
    return KEYWORD_TYPE_SIZE;
}

static bool is_symbol(string_view sv, size_t *length) {
    *length = 0;

    if (is_start_symbolic(*sv.start)) {
        while (sv.length-- && is_symbolic(*sv.start++)) {
            (*length)++;
        }
        return true;
    }

    return false;
}

static enum punctuation_type is_punctuation(string_view sv, size_t *length) {
    static_assert(PUNCTUATION_TYPE_SIZE == 11, "Not all punctuation_type values were handled");

    if (compare_prefix(sv, "\r\n", length)) { return LineEnd; }
    if (compare_prefix(sv, ":", length)) { return Colon; }
    if (compare_prefix(sv, "\n", length)) { return LineEnd; }
    if (compare_prefix(sv, ";", length)) { return Semicolon; }
    if (compare_prefix(sv, "{", length)) { return ScopeOpen; }
    if (compare_prefix(sv, "}", length)) { return ScopeClose; }
    if (compare_prefix(sv, "(", length)) { return ParenthesesOpen; }
    if (compare_prefix(sv, ")", length)) { return ParenthesesClose; }
    if (compare_prefix(sv, "[", length)) { return BracketOpen; }
    if (compare_prefix(sv, "]", length)) { return BracketClose; }
    if (compare_prefix(sv, ",", length)) { return Comma; }
    if (compare_prefix(sv, ".", length)) { return Dot; }

    *length = 0;
    return PUNCTUATION_TYPE_SIZE;
}
