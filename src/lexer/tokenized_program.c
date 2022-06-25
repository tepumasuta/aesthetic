#include "stdint.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "assert.h"

#include "sv/sv.h"

#include "token.h"
#include "tokenized_program.h"
#include "temp_lib_inst/darray/darray_token.h"
#include "temp_lib_inst/darray/darray_segment.h"
#include "data_structures/segment.h"


// Takes control of memory of darray pointer and text pointer
tokenized_program *tokenized_program_from_tokens(char *text, darray_token *tokens) {
    tokenized_program *tk_prog = malloc(sizeof(tokenized_program));
    tk_prog->tokens = tokens;

    darray_segment *segs_tmp = darray_segment_create(tokens->size, false);

    for (size_t i = 0; i < tokens->size; i++) {
        token t = tokens->arr[i];
        bool with_contents = true;
        string_view sv;
        switch (t.type) {
        case Value:
            sv = t.val.contents;
            break;
        case Symbol:
            sv = t.sym.contents;
            break;
        default:
            with_contents = false;
            break;
        }
        if (!with_contents)
            continue;
        
        size_t start = sv.data - text;
        segment seg = segment_create(start, start + sv.count - 1);
        size_t new_size = segment_unite(segs_tmp->arr, segs_tmp->size, seg);
        segs_tmp->size = new_size;
    }

    size_t total_size = 0;
    for (size_t i = 0; i < segs_tmp->size; i++)
        total_size += segs_tmp->arr[i].end + 1 - segs_tmp->arr[i].start;
    
    char *new_text_memory = malloc(sizeof(char) * (total_size + 1));
    new_text_memory[total_size] = '\0';
    char *last_pos_write = new_text_memory;
    for (size_t i = 0, j = 0; i < segs_tmp->size; i++) {
        enum token_type last_token_type = tk_prog->tokens->arr[j].type;
        while (j < tk_prog->tokens->size && last_token_type != Value && last_token_type != Symbol) {
            last_token_type = tk_prog->tokens->arr[++j].type;
        }

        switch (last_token_type) {
            case Value:
                tk_prog->tokens->arr[j++].val.contents.data = last_pos_write;
                break;
            case Symbol:
                tk_prog->tokens->arr[j++].sym.contents.data = last_pos_write;
                break;
            default:
                assert(0 && "Unreachable in tokenized_program");
                break;
        }

        size_t write_amount = segs_tmp->arr[i].end + 1 - segs_tmp->arr[i].start;

        memcpy(
            last_pos_write,
            text + segs_tmp->arr[i].start,
            write_amount
        );
        last_pos_write += write_amount;
    }

    darray_segment_destory(segs_tmp);
    free(text);
    tk_prog->tokens_text_memory = new_text_memory;

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
