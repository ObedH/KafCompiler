#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "defs.h"
#include "string.h"
#include "token.h"
#include "util.h"
#include "token_util.h"

#define CUR (*p)
#define NEXT (*(p+1))
#define CONSUME() do { p++; col ++; } while(0)

// ----------Token----------

const char* tt_str(TokenType tt) {
	switch(tt) {
		case TT_IDENTIFIER:
			return "IDENTIFIER";
		case TT_KEYWORD:
			return "KEYWORD";
		case TT_INT_LITERAL:
			return "INT LITERAL";
		case TT_FLOAT_LITERAL:
			return "FLOAT LITERAL";
		case TT_STRING_LITERAL:
			return "STRING LITERAL";
		case TT_CHAR_LITERAL:
			return "CHAR LITERAL";
		case TT_BOOL_LITERAL:
			return "BOOL LITERAL";
		case TT_PUNCT:
			return "PUNCTUATOR";
		case TT_OPERATOR:
			return "OPERATOR";
		default:
			return "UNKNOWN";
	}
}

Token token_create(TokenType tt, const char* lexeme, usize lexeme_len, usize col, usize line) {
	Token token = {0};
	token.token_type = tt;
	token.lexeme = string_create((char*)lexeme, lexeme_len);
	token.col = col;
	token.line = line;
	return token;
}
void token_print(Token token, usize level) {
	ptabs(level);
	printf("-----TOKEN-----\n");
	ptabs(level);
	printf("Token Type: %s\n", tt_str(token.token_type));
	ptabs(level);
	printf("Lexeme: \"");
	safe_print(token.lexeme.data, token.lexeme.length);
	printf("\"\n");
	ptabs(level);
	printf("Line: %u, Col: %u\n", token.line, token.col);
	ptabs(level);
	printf("---------------\n");
}
void token_free(Token token) {
	string_free(token.lexeme);
}

// ----------Token List----------

TokenList* token_list_create(void) {
	TokenList* tl = malloc(sizeof(TokenList));
	if(!tl) {
		perror("Could not mallocate space for token list");
		return NULL;
	}
	memset(tl, 0, sizeof(TokenList));
	return tl;
}
void token_list_init(TokenList* token_list) {
	token_list->count = 0;
	token_list->capacity = 16;
	token_list->head = malloc(sizeof(Token) * token_list->capacity);
	if(!token_list->head) {
		perror("Could not mallocate space for token list contents");
		token_list->head = NULL;
	}
}

void token_list_add(TokenList* token_list, Token token) {
	if(token_list->count == token_list->capacity){
		token_list->capacity += 16;
		void* tmp = realloc(token_list->head, sizeof(*token_list->head) * token_list->capacity);
		if(!tmp) {
			perror("Could not reallocate space for a new token!");
			return;
		}
		token_list->head = tmp;
	}
	// Will add
	token_list->head[token_list->count] = token;
	token_list->count ++;
	return;
}

void token_list_print(TokenList* token_list, usize level) {
	ptabs(level);
	printf("-----TOKEN LIST-----\n");
	ptabs(level);
	printf("List Length = %u\n", token_list->count);
	ptabs(level);
	printf("List Capacity= %u\n", token_list->capacity);
	ptabs(level);
	printf("\n");
	for(usize i = 0; i < token_list->count; i ++) {
		token_print(token_list->head[i], level + 1);
	}
	ptabs(level);
	printf("--------------------\n");
}

void token_list_free(TokenList* token_list) {
	for(usize i = 0; i < token_list->count; i ++) {
		token_free(token_list->head[i]);
	}
	free(token_list->head);
	free(token_list);
	return;
}

Lexer* lexer_create(void) {
	Lexer* tmp = malloc(sizeof(*tmp));
	if(!tmp) {
		perror("Failed to allocate memory for Lexer!");
		return NULL;
	}
	return tmp;
}

static inline void advance(Lexer* l) {
	if(*l->p == '\n') {
		l->line ++;
		l->col = 1;
	}
	else {
		l->col ++;
	}
	l->p++;
}

/* -------------------------- SCANNING ---------------------------- */

static void scan_whitespace(Lexer* l) {
	while(*l->p && isspace((unsigned char)*l->p)) {
		advance(l);
	}
}

static Token scan_identifier_or_keyword(Lexer* l) {
	const char* start = l->p;
	int line = l->line;
	int col = l->col;

	if(!isalpha(*l->p) && *l->p != '_') {
		return (Token){.token_type = TT_UNKNOWN};
	}

	while(isalnum(*l->p) || *l->p == '_') {
		advance(l);
	}

	usize len = l->p - start;

	if(is_bool_literal(start, len)) {
		return token_create(TT_BOOL_LITERAL, start, len, col, line);
	}

	if(is_keyword(start, len)) {
		return token_create(TT_KEYWORD, start, len, col, line);
	}

	return token_create(TT_IDENTIFIER, start, len, col, line);
}

static Token scan_operator(Lexer* l) {
	const char* start = l->p;
	int line = l->line;
	int col = l->col;

	usize best_len = 0;

	for(usize i = 0; i < NUM_OPERATORS; i ++) {
		const char* op = operators[i];
		usize len = strlen(op);

		if(strncmp(l->p, op, len) == 0) {
			if(len > best_len) {
				best_len = len;
			}
		}
	}

	if(best_len == 0) {
		return (Token){.token_type = TT_UNKNOWN};
	}
	
	for(usize i = 0; i < best_len; i ++) {
		advance(l);
	}

	return token_create(TT_OPERATOR, start, best_len, col, line);
}

static Token scan_punctuator(Lexer* l) {
	const char* start = l->p;
	int line = l->line;
	int col = l->col;

	for(usize i = 0; i < NUM_PUNCTUATORS; i ++) {
		const char* punc = punctuators[i];
		usize len = strlen(punc);

		if(strncmp(l->p, punc, len) == 0) {
			for(usize j = 0; j < len; j ++) {
				advance(l);
			}

			return token_create(TT_PUNCT, start, len, col, line);
		}
	}

	return (Token){.token_type = TT_UNKNOWN};
}

static Token scan_number_literal(Lexer* l) {
	const char* start = l->p;
	int line = l->line;
	int col = l->col;

	if(!isdigit(*l->p)) {
		return (Token) {.token_type = TT_UNKNOWN};
	}

	while(isdigit(*l->p)) {
		advance(l);
	}

	if(*l->p == '.' && isdigit(*(l->p + 1))) {
		advance(l);
		while(isdigit(*l->p)) {
			advance(l);
		}

		return token_create(TT_FLOAT_LITERAL, start, l->p - start, col, line);
	}

	return token_create(TT_INT_LITERAL, start, l->p - start, col, line);
}

static Token scan_char_literal(Lexer* l) {
	const char* start = l->p;
	int line = l->line;
	int col = l->col;

	if(*l->p != '\'') {
		return (Token) {.token_type = TT_UNKNOWN};
	}
	advance(l);
	advance(l);
	if(*l->p != '\'') {
		return (Token) {.token_type = TT_UNKNOWN};
	}
	advance(l);

	return token_create(TT_CHAR_LITERAL, start, l->p - start, col, line);
}

static Token scan_string_literal(Lexer* l) {
	if(*l->p != '"') {
		return (Token){.token_type = TT_UNKNOWN};
	}

	const char* start = l->p;
	int line = l->line;
	int col = l->col;

	advance(l);

	while(*l->p && *l->p != '"') {
		if(*l->p == '\\') advance(l);
		advance(l);
	}

	if(*l->p == '"') {
		advance(l);
	}

	return token_create(TT_STRING_LITERAL, start, l->p - start, col, line);
}

/* ---------------------------------------------------------------- */

TokenList* lexer_lex_src(Lexer* l, SourceFile* src) {
	l->src = src;
	l->p = src->file_contents;
	l->line = 1;
	l->col = 1;

	TokenList* list = token_list_create();
	token_list_init(list);


	while(*l->p) {
		if(isspace(*l->p)) {
			scan_whitespace(l);
			continue;
		}

		Token t;

		if((t = scan_identifier_or_keyword(l)).token_type != TT_UNKNOWN) {
			token_list_add(list, t);
			continue;
		}

		if((t = scan_number_literal(l)).token_type != TT_UNKNOWN) {
			token_list_add(list, t);
			continue;
		}

		if((t = scan_string_literal(l)).token_type != TT_UNKNOWN) {
			token_list_add(list, t);
			continue;
		}

		if((t = scan_char_literal(l)).token_type != TT_UNKNOWN) {
			token_list_add(list, t);
			continue;
		}
		
		if((t = scan_operator(l)).token_type != TT_UNKNOWN) {
			token_list_add(list, t);
			continue;
		}

		if((t = scan_punctuator(l)).token_type != TT_UNKNOWN) {
			token_list_add(list, t);
			continue;
		}

		advance(l);
	}

	return list;
}
void lexer_free(Lexer* lexer) {
	free(lexer);
}

/*
TokenList* tokenize(char* input) {

	// Initialize token list
	TokenList* token_list = token_list_create();
	token_list_init(token_list);

	// Initialize counters and char pointer
	char* p = input;
	usize col = 1;
	usize line = 1;

	// Loop through input string
	while(*p) {
		char* start = p;

		// NEWLINE
		if(CUR == '\n') {
			p ++;
			line ++;
			col = 0;
			continue;
		}
		
		// IDENTIFIER / KEYWORD
		if(isalpha(CUR) || CUR == '_') {
			start = p;
			CONSUME();

			while(isalnum(CUR) || CUR == '_') {
				CONSUME();
			}

			usize len = p - start;
			
			if(is_keyword(start, len)) {
				Token t = token_create(TT_KEYWORD, start, len, col, line);
				token_list_add(token_list, t);
			}
			else {
				Token t = token_create(TT_IDENTIFIER, start, len, col, line);
				token_list_add(token_list, t);
			}
			continue;
		}

		// PUNCTUATOR
		usize punctuator_len = 0;
		if(is_punctuator(p, &punctuator_len)) {
			Token t = token_create(TT_PUNCT, start, punctuator_len, col, line);
			token_list_add(token_list, t);
			p += punctuator_len;
			col += punctuator_len;
			continue;
		}

		// OPERATOR
		usize operator_len = 0;
		if(is_operator(p, &operator_len)) {
			Token t = token_create(TT_OPERATOR, start, operator_len, col, line);
			token_list_add(token_list, t);
			p += operator_len;
			col += operator_len;
			continue;
		}

		// NUMBER LITERAL
		usize num_len = 0;
		if(is_number_literal(p, &num_len)) {
			Token t = token_create(TT_CONSTANT, start, num_len, col, line);
			token_list_add(token_list, t);
			p += num_len;
			col += num_len;
			continue;
		}

		// DEFAULT
		CONSUME();
	}

	return token_list;
}
*/
