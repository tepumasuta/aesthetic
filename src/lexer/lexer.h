#ifndef LEXER_H
#define LEXER_H

#include "token.h"

typedef struct LEXER_impl LEXER;


LEXER *lexer_from_text(char *text);
token lex_token(LEXER *lexer);
void lexer_destroy(LEXER *lexer);

#endif // LEXER_H
