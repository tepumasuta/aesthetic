#include "token.h"
#include "stdio.h"

#include "assert.h"

static_assert(TOKEN_TYPE_SIZE == 3, "Not all token_type values are handled");
static const char* TOKEN_TYPE_CONVERT_TABLE[] = {"Operator", "Value", "Keyword"};
static_assert(OPERATION_TYPE_SIZE == 6, "Not all operation_type values are handled");
static const char* OPERATION_TYPE_CONVERT_TABLE[] = {"+", "-", "*", "/", "//", "="};
static_assert(VALUE_TYPE_SIZE == 2, "Not all value_type values are handled");
static const char* VALUE_TYPE_CONVERT_TABLE[] = {"int", "float"};
static_assert(INTEGER_LITERAL_TYPE_SIZE == 4, "Not all integer_literal_type values are handled");
static const char* INTEGER_LITERAL_TYPE_CONVERT_TABLE[] = {"Hex", "Oct", "Bin", "Dec"};
static_assert(KEYWORDS_SIZE == 1, "Not all keyword_type values are handled");
static const char* KEYWORD_TYPE_CONVERT_TABLE[] = {"if"};

void print_token(token t) {
    if (!t.valid) {
        puts("Token{Invalid}");
        return;
    }
    
    printf("Token{%s}", TOKEN_TYPE_CONVERT_TABLE[t.type]);
    switch (t.type) {
        case Operator:
            printf(" `%s` %ld", OPERATION_TYPE_CONVERT_TABLE[t.op.op_type], t.op.length);
            break;
        case Value:
            printf(" %s: `"SV_Fmt"`", VALUE_TYPE_CONVERT_TABLE[t.val.val_type], SV_Arg(t.val.contents));
            switch (t.val.val_type) {
            case Integer:
                printf(" %s", INTEGER_LITERAL_TYPE_CONVERT_TABLE[t.val.int_lit.int_lit_type]);
                break;
            case FloatingPoint:
                printf(" float");
                break;
            case VALUE_TYPE_SIZE:
                assert(0 && "Wrong value token type in `print_token`");
                break;
            }
            break;
        case Keyword:
            printf(" `%s`", KEYWORD_TYPE_CONVERT_TABLE[t.kw.kw_type]);
            break;
        case TOKEN_TYPE_SIZE:
            assert(0 && "Wrong token type in `print_token`");
            break;
    }
    putc('\n', stdout);
}
