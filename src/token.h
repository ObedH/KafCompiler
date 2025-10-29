#ifndef TOKEN_H
#define TOKEN_H

#include <stdint.h>

/*--------------------------------------
 * ENUM: Token types
 *-------------------------------------*/
enum e_token_type {
    TOKEN_TYPE_KEYWORD,
    TOKEN_TYPE_IDENTIFIER,
    TOKEN_TYPE_CONSTANT,
    TOKEN_TYPE_STRING_LITERAL,
    TOKEN_TYPE_PUNCTUATOR,
    TOKEN_TYPE_UNKNOWN
};

extern const char* token_types[];

/*--------------------------------------
 * STRUCT: Token
 *-------------------------------------*/
struct s_token {
    enum e_token_type token_type;
    char *lexeme;
    uint32_t line_number;
    uint32_t column_number;
    struct s_token *next;
};

/*--------------------------------------
 * LINKED LIST
 *--------------------------------------*/
typedef struct s_token *token_list_t;

/*--------------------------------------
 * FUNCTION POINTER TYPES
 *-------------------------------------*/
typedef void (*t_token_list_iterator)(struct s_token *p_node);

/*--------------------------------------
 * FUNCTION DECLARATIONS
 *-------------------------------------*/
struct s_token token_init(enum e_token_type token_type, const char *lexeme, uint32_t line_number, uint32_t column_number);

void token_print(const struct s_token *p_token);

token_list_t token_list_init(void);
void token_list_add(token_list_t *p_token_list, const struct s_token *p_token);
void token_list_remove(token_list_t *p_token_list, const struct s_token *p_token);
void token_list_iterate(token_list_t token_list, t_token_list_iterator callback);
void token_list_print_contents(token_list_t token_list);

token_list_t tokenize(const char *input_string);

#endif /* TOKEN_H */