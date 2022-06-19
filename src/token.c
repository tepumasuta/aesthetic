#include "stdio.h"
#include "assert.h"

#include "token.h"

#ifdef AE_DEBUG
static_assert(TOKEN_TYPE_SIZE == 5, "Not all token_type values are handled");
static const char* TOKEN_TYPE_CONVERT_TABLE[] = {"Operator", "Value", "Keyword", "Symbol", "EndOfFile"};
static_assert(OPERATION_TYPE_SIZE == 8, "Not all operation_type values are handled");
static const char* OPERATION_TYPE_CONVERT_TABLE[] = {"+", "-", "*", "/", "//", "=", "::=", ":="};
static_assert(VALUE_TYPE_SIZE == 2, "Not all value_type values are handled");
static const char* VALUE_TYPE_CONVERT_TABLE[] = {"int", "float"};
static_assert(INTEGER_LITERAL_TYPE_SIZE == 4, "Not all integer_literal_type values are handled");
static const char* INTEGER_LITERAL_TYPE_CONVERT_TABLE[] = {"Hex", "Oct", "Bin", "Dec"};
static_assert(KEYWORD_TYPE_SIZE == 1, "Not all keyword_type values are handled");
static const char* KEYWORD_TYPE_CONVERT_TABLE[] = {"if"};

void print_token(token t) {
    if (!t.valid) {
        puts("Token{Invalid}");
        return;
    }
    
    printf("Token[%ld:%ld]{%s}", t.pos.line, t.pos.col, TOKEN_TYPE_CONVERT_TABLE[t.type]);
    switch (t.type) {
        case Operator:
            printf(" %ld `%s`", t.op.length, OPERATION_TYPE_CONVERT_TABLE[t.op.op_type]);
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
        case Symbol:
            printf(" `"SV_Fmt"`", SV_Arg(t.sym.contents));
            break;
        case EndOfFile:
            break;
        case TOKEN_TYPE_SIZE:
            assert(0 && "Wrong token type in `print_token`");
            break;
    }
    putc('\n', stdout);
}
#else
void print_token(token t) {
    (void)t;
}
#endif
