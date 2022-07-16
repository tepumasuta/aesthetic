#include "stddef.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"
#include "assert.h"

#include "token.h"
#include "data_structures/string_view.h"
#include "lexer.h"
#include "utility/character.h"

typedef struct LEXER_impl {
    position current_pos;
    char *start;
    string_view text_pos;
} LEXER;

static enum operation_type is_operator(string_view sv, size_t *length);
static enum keyword_type is_keyword(string_view sv, size_t *length);
static enum punctuation_type is_punctuation(string_view sv, size_t *length);
static enum value_type is_value(string_view sv);
static enum integer_literal_type is_integer_literal(string_view sv);
static bool is_symbol(string_view sv, size_t *length);

static void parse_integer_literal(string_view sv, size_t *size, string_view *contents, enum integer_literal_type int_lit);
static enum integer_literal_type parse_number(string_view sv, size_t *size, string_view *contents);
static void parse_string(string_view sv, size_t *size, position *pos, string_view *contents);

static void convert_to_number(LEXER *lexer, token *t, string_view contents, size_t size, enum integer_literal_type int_lit) {
    t->valid = true;
    t->type = Value;
    t->val.contents = contents;
    t->val.val_type = Number;

    if (int_lit == INTEGER_LITERAL_TYPE_SIZE) {
        t->val.num.num_type = FloatingPoint;
    } else {
        t->val.num.num_type = Integer;
        t->val.num.int_lit = int_lit;
    }

    sv_step(&lexer->text_pos, size);
    lexer->current_pos.col += size;
}

static void convert_to_string(LEXER *lexer, token *t, string_view contents, size_t size, size_t length, position taken) {
    t->valid = true;
    t->type = Value;
    t->val.contents = contents;
    t->val.val_type = String;
    t->val.str_lit.length = length;

    sv_step(&lexer->text_pos, size);
    if (!taken.line)
        lexer->current_pos.col += size;
    else {
        lexer->current_pos.line += taken.line;
        lexer->current_pos.col = taken.col;
    }
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

    size_t size;

    enum value_type val_type;
    if ((val_type = is_value(lexer->text_pos)) != VALUE_TYPE_SIZE) {
        string_view contents;
        enum integer_literal_type int_lit;
        position taken;

        switch (val_type) {
            case Number:
                int_lit = is_integer_literal(lexer->text_pos);
                if (int_lit != INTEGER_LITERAL_TYPE_SIZE)
                    parse_integer_literal(lexer->text_pos, &size, &contents, int_lit);
                else
                    int_lit = parse_number(lexer->text_pos, &size, &contents);
                convert_to_number(lexer, &t, contents, size, int_lit);
                break;
            case String:
                parse_string(lexer->text_pos, &size, &taken, &contents);
                convert_to_string(lexer, &t, contents, size, size - 2, taken);
                break;
            case VALUE_TYPE_SIZE:
                assert(0 && "Unreachable in lex_token. value_type unexpected");
                break;
        }
        return t;
    }

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
            lexer->current_pos.col = 1;
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
        if (length)
            *length = count;
        return true;
    }

    return false;
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

static enum value_type is_value(string_view sv) {
    if (is_digit(*sv.start)) { return Number; }
    if (is_string_bound(*sv.start)) { return String; }

    return VALUE_TYPE_SIZE;
}

static enum integer_literal_type is_integer_literal(string_view sv) {
    if (compare_prefix(sv, "0x", NULL)) { return Hex; }
    if (compare_prefix(sv, "0o", NULL)) { return Oct; }
    if (compare_prefix(sv, "0b", NULL)) { return Bin; }

    return INTEGER_LITERAL_TYPE_SIZE;
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


static void parse_integer_literal(string_view sv, size_t *size, string_view *contents, enum integer_literal_type int_lit) {
    assert(int_lit < 3);

    static bool (*check_functions[]) (char) = {is_hex, is_oct, is_bin};
    bool (*check_char) (char) = check_functions[int_lit];

    char *pos = sv.start;
    pos += 2;

    while (check_char(*(pos++)));
    pos--;

    *size = pos - sv.start;

    contents->start = sv.start;
    contents->length = *size;
}

static enum integer_literal_type parse_number(string_view sv, size_t *size, string_view *contents) {
    char *pos = sv.start;
    
    while (is_digit(*(pos++)));
    
    pos--;

    if (*(pos) == '.') {
        while (is_digit(*(++pos)));

        *size = pos - sv.start;

        contents->start = sv.start;
        contents->length = *size;

        return INTEGER_LITERAL_TYPE_SIZE;
    }

    *size = pos - sv.start;

    contents->start = sv.start;
    contents->length = *size;

    return Dec;
}

static void parse_string(string_view sv, size_t *size, position *pos, string_view *contents) {
    char opening = *sv.start;
    char *cur = sv.start;

    pos->col = 0;
    pos->line = 0;

    bool escaped = false;

    while (*cur && (*(++cur) != opening || escaped)) {
        escaped = *cur == '\\';

        pos->col++;

        if (*cur == '\n') {
            pos->col = 0;
            pos->line++;
        }
    }

    *size = cur - sv.start + 1;

    contents->start = sv.start;
    contents->length = *size;
}
