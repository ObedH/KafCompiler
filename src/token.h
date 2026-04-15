#ifndef TOKEN_H
#define TOKEN_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "string.h"
#include "source.h"

typedef struct {
	SourceFile* src;
	const char* start;
	const char* current;
	int line;
	int col;
} TokenScanner;

typedef enum {
	TT_IDENTIFIER,
	TT_KEYWORD,
	TT_CONSTANT,
	TT_STRING_LITERAL,
	TT_PUNCT,
	TT_OPERATOR,
	TT_UNKNOWN
} TokenType;
const char* tt_str(TokenType tt);

typedef struct {
	TokenType token_type;
	String lexeme;
	usize line, col;
} Token;
Token token_create(TokenType tt, char* lexeme, usize lexeme_len, usize col, usize line);
void token_print(Token token, usize level);
void token_free(Token token);

typedef struct {
	Token* head;
	usize count;
	usize capacity;
} TokenList;
TokenList* token_list_create(void);
void token_list_init(TokenList* token_list);
void token_list_print(TokenList* token_list, usize level);
void token_list_free(TokenList* token_list);

TokenList* tokenize(char* input);

TokenScanner* tokscan_create(void);
TokenList* tokscan_scan_src(TokenScanner* tokscan, SourceFile* src_file);
void tokscan_free(TokenScanner* tokscan);

#endif // TOKEN_H
