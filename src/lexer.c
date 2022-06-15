#include "token.h"
#include "lexer.h"
#include "sv/sv.h"

typedef struct LEXER_impl {
    char *start;
    string_view pos;
} LEXER;

LEXER* lexer_from_text(const char *text) {
    LEXER* lexer = malloc(sizeof(LEXER));

    if (!lexer) {
        return NULL;
    }

    // lexer->start = strdup(text);
    size_t text_length = (strlen(text) + 1);
    lexer->start = (char *)malloc(text_length * sizeof(char));
    memcpy(lexer->start, text, text_length);

    lexer->pos = sv_from_cstr(lexer->start);

    return lexer;
}

void lexer_destroy(LEXER* lexer) {
    free(lexer->start);
    free(lexer);
}
