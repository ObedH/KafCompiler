#ifndef PARSE_H
#define PARSE_H

#include "defs.h"
#include "token.h"
#include "ast.h"
#include <string.h>
#include <stdio.h>

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
static inline Token* prev(Parser* p) {
	if(p->pos > 0) {
		return &p->tokens->head[p->pos - 1];
	}
	return NULL;
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
static inline bool match_op(Parser* p, const char* op) {
	Token* t = peek(p);
	if(t && t->token_type == TT_OPERATOR && !strcmp(t->lexeme.data, op)) {
		p->pos ++;
		return true;
	}
	return false;
}
static inline void expect_punct(Parser* p, const char* punctuator) {
	Token* t = peek(p);
	if(t) {
		if(t->token_type == TT_PUNCT && !strcmp(t->lexeme.data, punctuator)) {
			p->pos ++;
		}
		else {
			printf("-----SYNTAX ERROR-----\n");
			printf("Expected token %s\n", punctuator);
			printf("But found %s instead!\n", t->lexeme.data);
			printf("----------------------\n");
		}
	}
}
static inline bool check_punct(Parser* p, const char* punctuator) {
	Token* t = peek(p);
	return (t && t->token_type == TT_PUNCT && !strcmp(t->lexeme.data, punctuator));
}
static inline bool check_op(Parser* p, const char* op) {
	Token* t = peek(p);
	return (t && t->token_type == TT_OPERATOR && !strcmp(t->lexeme.data, op));
}

Parser* parser_create(void);
void parser_init(Parser* parser, TokenList* tokens);
void parser_free(Parser* parser);

ASTNode* parse(TokenList* tokens);
ASTNode* parse_decl(Parser* p);
ASTNode* parse_type(Parser* p);
ASTNode* parse_param(Parser* p);
ASTNode* parse_block(Parser* p);
ASTNode* parse_stmt(Parser* p);
ASTNode* parse_expr(Parser* p);
ASTNode* parse_assignment(Parser* p);
ASTNode* parse_log_or(Parser* p);
ASTNode* parse_log_xor(Parser* p);
ASTNode* parse_log_and(Parser* p);
ASTNode* parse_equality(Parser* p);
ASTNode* parse_comparison(Parser* p);
ASTNode* parse_term(Parser* p);
ASTNode* parse_factor(Parser* p);
ASTNode* parse_unary(Parser* p);
ASTNode* parse_primary(Parser* p);


#endif
