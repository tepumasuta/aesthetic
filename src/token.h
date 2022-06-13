#ifndef TOKEN_H
#define TOKEN_H

enum token_type {
    Operation=0,
    TOKEN_TYPE_SIZE
};

typedef struct {
    enum token_type type;
    const char *contents;
    union {

    };
} token;

void print_token(token t);

#endif // TOKEN_H
