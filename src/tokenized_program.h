#ifndef TOKENIZED_PROGRAM_H
#define TOKENIZED_PROGRAM_H

#include "token.h"
#include "darray.h"

typedef struct {
    char *tokens_text_memory;
    darray *tokens;
} tokenized_program;

tokenized_program *tokenized_program_from_tokens(char *text, darray *tokens);
void tokenized_program_destroy(tokenized_program *tk_prog);
void print_tokenized_program(tokenized_program *tk_prog);

#endif // TOKENIZED_PROGRAM_H
