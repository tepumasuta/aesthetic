#include "token.h"

#ifndef LEXER_H
#define LEXER_H

typedef struct LEXER_impl LEXER;

LEXER* lexer_from_text(const char* text);
token lex_token(LEXER* lexer);
void lexer_destroy(LEXER* lexer);

#endif // LEXER_H
