#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

const char* token_types[] = {
  "KEYWORD",
  "IDENTIFIER",
  "CONSTANT",
  "STRING_LITERAL",
  "PUNCTUATOR",
  "UNKNOWN"
};

struct s_token token_init(enum e_token_type token_type, const char *lexeme, uint32_t line_number, uint32_t column_number){
    struct s_token token;
    token.token_type = token_type;
    token.line_number = line_number;
    token.column_number = column_number;

    if(lexeme != NULL){
        size_t lexeme_len = strnlen(lexeme, 64);
        char *temp_ptr = malloc(lexeme_len * sizeof(char));
        if(temp_ptr != NULL){
            memcpy(temp_ptr, lexeme, lexeme_len + 1);
            token.lexeme = temp_ptr;
        }
        else {
            fprintf(stderr, "Cannot allocate memory for a lexeme copy! (at %d:%d, the lexeme was %s)\n", line_number, column_number, lexeme);
            token.lexeme = NULL;
        }
    }
    else {
        fprintf(stderr, "Lexeme is NULL! at %d:%d\n", line_number, column_number);
        token.lexeme = NULL;
    }

    return token;
}

void token_print(const struct s_token *p_token) {
    printf("-------TOKEN-------\n");

    if(p_token->lexeme == NULL){
        fprintf(stderr, "LEXEME IS NULL!\n");
    }
    else {
        printf(" \"%s\"\n", p_token->lexeme);
    }

    if(p_token->token_type >= 6){
        fprintf(stderr, " INVALID TOKEN TYPE: %d\n", p_token->token_type);
    }
    else {
        printf(" %s\n", token_types[p_token->token_type]);
    }

    printf(" At %d:%d\n\n", p_token->line_number, p_token->column_number);
    printf("--------------------\n");
}

token_list_t token_list_init(void){
    return NULL;
}

void token_list_add(token_list_t *p_token_list, const struct s_token *p_token){
    if (p_token_list == NULL){
        fprintf(stderr, "CANNOT FIND TOKEN LIST");
        return;
    }
    if (p_token == NULL){
        fprintf(stderr, "CANNOT FIND TOKEN TO ADD");
        return;
    }

    struct s_token *p_new_node = malloc(sizeof(struct s_token));

    if(p_new_node == NULL){
        fprintf(stderr, "CANNOT ALLOCATE MEMORY FOR NEW TOKEN!");
        return;
    }
}


/*t_token_list *token_list_init(void);
void token_list_add(t_token_list **pp_token_list, const struct s_token *p_token);
void token_list_remove(t_token_list **pp_token_list, const struct s_token *p_token);
void token_list_iterate(t_token_list *p_token_list, t_token_list_iterator callback);
void token_list_print_contents(t_token_list *p_token_list);

t_token_list *tokenize(const char *input_string);*/