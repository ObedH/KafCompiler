#include "parse.h"
#include <stdio.h>
#include "ast.h"
#include <stdlib.h>

Parser* parser_create(void) {
	Parser* p = malloc(sizeof(Parser));
	if(!p) {
		perror("Failed to mallocate memory for a Parser!");
		return NULL;
	}
	memset(p, 0, sizeof(Parser));
	return p;
}
void parser_init(Parser* p, TokenList* tokens) {
	p->tokens = tokens;
}

ASTNode* parse(TokenList* tokens) {
	Parser* p = parser_create();
	parser_init(p, tokens);
	ASTNode* program_node = ast_program_node_create();
	while(p->pos < p->tokens->count) {
		ASTNode* a = parse_decl(p);
		if(!a) break;
		ast_program_node_append(program_node, a);
	}
	parser_free(p);

	return program_node;
}
ASTNode* parse_decl(Parser* p) {
	bool public = match_keywrd(p, "pub");
	(void)public;
	if(match_keywrd(p, "fn")) {
		ASTNode* fun = ast_func_node_create();
		// I expect a : Nothing else is valid here.
		if(!match_punct(p, ":")) {
			printf("Syntax error, yeah yeah yeah\n");
		}
		ASTNode* return_type = parse_type(p);
		Token* fn_name_token = next(p);
		String fn_name = fn_name_token->lexeme;
		
		expect_punct(p, "(");
		if(!check_punct(p, ")")) {
			do {
				ASTNode* param = parse_param(p);
				ast_func_node_add_param(fun, param);
			} while(match_punct(p, ","));
		}

		expect_punct(p, ")");

		if(!match_punct(p, "{")) {
			printf("Syntax error, yeah yeah yeah\n");
		}
		ASTNode* fn_body = parse_block(p);
		ast_func_node_init(fun, fn_name, fn_body, return_type);
		return fun;
	}
	return NULL;
}
ASTNode* parse_type(Parser* p) {
	ASTNode* type = ast_type_node_create();
	Token* type_token = next(p);
	ast_type_node_init(type, type_token->lexeme);
	return type;
}
ASTNode* parse_param(Parser* p) {
	ASTNode* param = ast_param_node_create();
	Token* param_name = next(p);
	if(!match_punct(p, ":")) {
		printf("Syntax error, yeah yeah yeah\n");
	}
	ASTNode* param_type = parse_type(p);
	ast_param_node_init(param, param_name->lexeme, param_type);
	return param;
}
ASTNode* parse_block(Parser* p) {
	ASTNode* block = ast_block_node_create();
	while(!check_punct(p, "}")){
		ASTNode* stmt = parse_stmt(p);
		if(!stmt) continue;
		ast_block_node_add_stmt(block, stmt);
	}

	next(p);
	return block;
}
ASTNode* parse_stmt(Parser* p) {
	Token* t = peek(p);
	if(!strcmp(t->lexeme.data, "return")) {
		next(p); // Consume 'return'
		ASTNode* return_value = parse_expr(p);
		expect_punct(p, ";");
		ASTNode* return_node = ast_return_node_create();
		ast_return_node_init(return_node, return_value);
		return return_node;
	}
	return NULL;
}
ASTNode* parse_expr(Parser* p) {
	return parse_assignment(p);
}
ASTNode* parse_assignment(Parser* p) {
	ASTNode* left = parse_equality(p);

	if(match_op(p, "=")) {
		ASTNode* value = parse_assignment(p);
		return ast_assign_expr_node_create(left, value);
	}

	return left;
}
ASTNode* parse_equality(Parser* p) {
	ASTNode* left = parse_comparison(p);

	while(match_op(p, "==") || match_op(p, "!=")) {
		Token* op = prev(p);
		ASTNode* right = parse_comparison(p);
		left = ast_binary_expr_node_create(op->lexeme, left, right);
	}

	return left;
}
ASTNode* parse_comparison(Parser* p) {
	ASTNode* left = parse_term(p);

	while(match_op(p, ">") || match_op(p, "<") || match_op(p, ">=") || match_op(p, "<=")) {
		Token* op = prev(p);
		ASTNode* right = parse_term(p);
		left = ast_binary_expr_node_create(op->lexeme, left, right);
	}

	return left;
}
ASTNode* parse_term(Parser* p) {
	ASTNode* left = parse_factor(p);

	while(match_op(p, "+") || match_op(p, "-")) {
		Token* op = prev(p);
		ASTNode* right = parse_factor(p);
		left = ast_binary_expr_node_create(op->lexeme, left, right);
	}

	return left;
}
ASTNode* parse_factor(Parser* p) {
	ASTNode* left = parse_unary(p);

	while(match_op(p, "*") || match_op(p, "/")) {
		Token* op = prev(p);
		ASTNode* right = parse_unary(p);
		left = ast_binary_expr_node_create(op->lexeme, left, right);
	}
	
	return left;
}
ASTNode* parse_unary(Parser* p) {
	if(match_op(p, "-") || match_op(p, "!") || match_op(p, "~")) {
		Token* op = prev(p);
		ASTNode* right = parse_unary(p);
		return ast_unary_expr_node_create(op->lexeme, right);
	}

	return parse_primary(p);
}
/*
 * parse_expr
 * parse_assignment
 * parse_equality
 * parse_comparison
 * parse_term
 * parse_factor
 * parse_unary
 * parse_primary
 */


ASTNode* parse_primary(Parser* p) {
	Token* t = peek(p);
	
	if(t->token_type == TT_CONSTANT) {
		next(p);
		return ast_literal_int_node_create(atoi(t->lexeme.data));
	}
	else if(t->token_type == TT_IDENTIFIER) {
		next(p);
		return ast_identifier_node_create(t->lexeme);
	}
	else if(t->token_type == TT_PUNCT && strcmp(t->lexeme.data, "(") == 0) {
		next(p);
		ASTNode* expr = parse_expr(p);
		expect_punct(p, ")");
		return expr;
	}

	perror("Expected expression");
	return NULL;
}

void parser_free(Parser* p) {
	free(p);
}
