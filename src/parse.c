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
		ast_program_node_append(program_node, a);
		break;
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
		if(!match_punct(p, "(")) {
			printf("Syntax error, yeah yeah yeah\n");
		}
		do {
			ASTNode* param = parse_param(p);
			ast_func_node_add_param(fun, param);
			
		} while(match_punct(p, ","));
		next(p);
		if(!match_punct(p, "{")) {
			printf("Syntax error, yeah yeah yeah\n");
		}
		//ASTNode* fn_body = parse_compound_stmt(p);
		ASTNode* fn_body = NULL;
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


void parser_free(Parser* p) {
	free(p);
}
