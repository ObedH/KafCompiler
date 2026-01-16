#ifndef PARSE_H
#define PARSE_H

#include "defs.h"
#include "token.h"
#include "ast.h"
#include <string.h>

typedef struct {
	TokenList* tokens;
	usize pos; // Current position
} Parser;

static inline Token* peek(Parser* p) {
	if(p->pos < p->tokens->count) {
		return &p->tokens->head[p->pos];
	}
	return NULL;

}
static inline Token* next(Parser* p) {
	if(p->pos < p->tokens->count) {
		return &p->tokens->head[p->pos++];
	}
	return NULL;
}
static inline void prev(Parser* p) {
	if(p->pos > 0) {
		p->pos --;
	}
}
static inline bool match(Parser* p, TokenType kind) {
	Token* t = peek(p);
	if(t && t->token_type == kind) {
		p->pos ++;
		return true;
	}
	return false;
}
static inline bool match_keywrd(Parser* p, const char* keyword) {
	Token* t = peek(p);
	if(t && t->token_type == TT_KEYWORD && !strcmp(t->lexeme.data, keyword)) {
		p->pos ++;
		return true;
	}
	return false;
}
static inline bool match_punct(Parser* p, const char* punctuator) {
	Token* t = peek(p);
	if(t && t->token_type == TT_PUNCT && !strcmp(t->lexeme.data, punctuator)) {
		p->pos ++;
		return true;
	}
	return false;
}
Parser* parser_create(void);
void parser_init(Parser* parser, TokenList* tokens);
void parser_free(Parser* parser);

ASTNode* parse(TokenList* tokens);
ASTNode* parse_decl(Parser* p);
ASTNode* parse_type(Parser* p);
ASTNode* parse_param(Parser* p);


#endif
