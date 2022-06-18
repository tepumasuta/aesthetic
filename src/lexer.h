#include "token.h"

#ifndef LEXER_H
#define LEXER_H

typedef struct LEXER_impl LEXER;

// Creates and initializes LEXER structure
// Text content is mallocated into `content_holder`, whcih has to be
// freed when you are done with tokens. Tokens will get invalidated
// after freeing `content_holder`
LEXER *lexer_from_text(const char *text, char **content_holder);
// Lexes one token from LEXER. If cannot lex a token
// returns token with `valid` field set to false. In other cases
// `valid` filed is set to true
token lex_token(LEXER *lexer);
// Destroy LEXER. Doesn't deallocate mallocated text string put into `content_holder`
// upon construction
void lexer_destroy(LEXER *lexer);

#endif // LEXER_H
