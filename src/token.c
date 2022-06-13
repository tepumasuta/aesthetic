#include "token.h"
#include "stdio.h"

#include "assert.h"

static_assert(TOKEN_TYPE_SIZE == 1, "Not all token_type values are handled");
static const char* TOKEN_TYPE_CONVERT_TABLE[] = {"Operation"};

void print_token(token t) {
    printf("Token{%s} `%s`\n", TOKEN_TYPE_CONVERT_TABLE[t.type], t.contents);
}
