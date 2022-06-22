#include "stdint.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#include "sv/sv.h"

#include "token.h"
#include "darray.h"
#include "tokenized_program.h"

DARRAY_DEFINE(token)
DARRAY_DEFINE_PRINT(token)

// Takes control of memory of darray pointer and text pointer
tokenized_program *tokenized_program_from_tokens(char *text, darray_token *tokens) {
    tokenized_program *tk_prog = malloc(sizeof(tokenized_program));
    tk_prog->tokens = tokens;
    tk_prog->tokens_text_memory = text;

    return tk_prog;
}

void tokenized_program_destroy(tokenized_program *tk_prog) {
    free(tk_prog->tokens_text_memory);
    darray_token_destory(tk_prog->tokens);
    free(tk_prog);
}

#ifdef AE_DEBUG
void print_tokenized_program(tokenized_program *tk_prog) {
    puts("TokenizedProgram:");
    printf("~ raw data: \"%s\"\n", tk_prog->tokens_text_memory);
    fputs("~ ", stdout);
    print_darray_token(tk_prog->tokens);
}
#else
void print_tokenized_program(tokenized_program *tk_prog) {
    (void)tk_prog;
}
#endif
