#include "stdio.h"
#include "assert.h"

#include "token.h"

#ifdef AE_DEBUG
static_assert(TOKEN_TYPE_SIZE == 6, "Not all token_type values are handled");
static const char *TOKEN_TYPE_CONVERT_TABLE[] = {"Operator", "Value", "Keyword", "Symbol", "EndOfFile", "Punctuation"};
static_assert(VALUE_TYPE_SIZE == 2, "Not all value_type values are handled");
static const char *VALUE_TYPE_CONVERT_TABLE[] = {"num", "str"};
static_assert(NUMBER_TYPE_SIZE == 2, "Not all number_type values are handled");
static const char *NUMBER_TYPE_CONVERT_TABLE[] = {"int", "float"};
static_assert(INTEGER_LITERAL_TYPE_SIZE == 4, "Not all integer_literal_type values are handled");
static const char *INTEGER_LITERAL_TYPE_CONVERT_TABLE[] = {"Hex", "Oct", "Bin", "Dec"};
static_assert(OPERATION_TYPE_SIZE == 12, "Not all operation_type values are handled");
static const char *OPERATION_TYPE_CONVERT_TABLE[] = {
    "+", "-", "*", "/", "//", "=", "::=", ":=", "///", "~>", "!!", "~!"
};
static_assert(KEYWORD_TYPE_SIZE == 5, "Not all keyword_type values are handled");
static const char *KEYWORD_TYPE_CONVERT_TABLE[] = {"if", "when", "whenever", "exist", "on"};
static_assert(PUNCTUATION_TYPE_SIZE == 11, "Not all punctuation_type values are handled");
static const char *PUNCTUATION_TYPE_CONVERT_TABLE[] = {
    ";", "{", "}", "(", ")", "[", "]", ",", ".", "\\n", ":"
};

void print_token(token t) {
    if (!t.valid) {
        puts("Token{Invalid}");
        return;
    }
    
    printf("Token[%ld:%ld]{%s}", t.pos.line, t.pos.col, TOKEN_TYPE_CONVERT_TABLE[t.type]);
    switch (t.type) {
        case Operator:
            printf(" `%s`", OPERATION_TYPE_CONVERT_TABLE[t.op.op_type]);
            break;
        case Value:
            printf(" %s: `"SV_Fmt"`", VALUE_TYPE_CONVERT_TABLE[t.val.val_type], SV_Arg(t.val.contents));
            switch (t.val.val_type) {
                case Number:
                    printf(" [%s]", NUMBER_TYPE_CONVERT_TABLE[t.val.num.num_type]);
                    switch (t.val.num.num_type) {
                        case Integer:
                            printf(" %s", INTEGER_LITERAL_TYPE_CONVERT_TABLE[t.val.num.int_lit]);
                            break;
                        case FloatingPoint:
                            break;
                        case NUMBER_TYPE_SIZE:
                            assert(0 && "Wrong value number type in `print_token`");
                            break;
                    }
                    break;
                case String:
                    printf(" (%ld)", t.val.str_lit.length);
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
        case Punctuation:
            printf(" `%s`", PUNCTUATION_TYPE_CONVERT_TABLE[t.pt.pt_type]);
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
