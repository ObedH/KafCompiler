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

ASTNode* parser_parse(Parser* p, TokenList* tokens) {
	p->tokens = tokens;
	ASTNode* program_node = ast_program_node_create();
	while(p->pos < p->tokens->count) {
		ASTNode* a = parse_decl(p);
		if(!a) break;
		ast_program_node_append(program_node, a);
	}
	return program_node;
}
ASTNode* parse_decl(Parser* p) {
	bool public = match_keywrd(p, "export");
	(void)public;
	if(match_keywrd(p, "fn")) {
		ASTNode* fun = ast_func_node_create();
		expect_punct(p, ":");

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

		expect_punct(p, "{");

		ASTNode* fn_body = parse_block(p);
		ast_func_node_init(fun, fn_name, fn_body, return_type);
		return fun;
	}
	else if(match_keywrd(p, "var")) {
		ASTNode* var = ast_var_node_create();
		expect_punct(p, ":");

		ASTNode* type = parse_type(p);
		Token* var_name_token = next(p);
		String var_name = var_name_token->lexeme;

		ASTNode* initializer = NULL;
		if(match_op(p, "=")) {
			initializer = parse_expr(p);
		}
		expect_punct(p, ";");
		ast_var_node_init(var, var_name, type, initializer);
		return var;
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

	expect_punct(p, ":");

	ASTNode* param_type = parse_type(p);
	ast_param_node_init(param, param_name->lexeme, param_type);
	return param;
}
ASTNode* parse_block(Parser* p) {
	ASTNode* block = ast_block_node_create();
	while(!check_punct(p, "}")){
		ASTNode* stmt = parse_stmt(p);
		if(!stmt) {
			next(p); // Recovery
			continue;
		}
		ast_block_node_add_stmt(block, stmt);
	}

	next(p);
	return block;
}
ASTNode* parse_var_decl(Parser* p) {
	next(p);
	ASTNode* var = ast_var_node_create();
	expect_punct(p, ":");

	ASTNode* type = parse_type(p);
	Token* var_name_token = next(p);
	String var_name = var_name_token->lexeme;

	ASTNode* initializer = NULL;
	if(match_op(p, "=")) {
		initializer = parse_expr(p);
	}
	expect_punct(p, ";");
	ast_var_node_init(var, var_name, type, initializer);
	return var;
}
ASTNode* parse_for_stmt(Parser* p) {
	next(p);
	ASTNode* fo = ast_for_node_create();
	expect_punct(p, "(");

	ASTNode* init = NULL;
	if(check_keywrd(p, "var")) {
		init = parse_var_decl(p); // consumes ;
	}
	else if(!check_punct(p, ";")) {
		init = parse_expr(p);
		expect_punct(p, ";");
	}
	else {
		expect_punct(p, ";");
	}

	ASTNode* cond = NULL;
	if(!check_punct(p, ";")) {
		cond = parse_expr(p);
	}
	expect_punct(p, ";");

	ASTNode* update = NULL;
	if(!check_punct(p, ")")) {
		update = parse_expr(p);
	}
	expect_punct(p, ")");

	ASTNode* body = parse_stmt(p);

	ast_for_node_init(fo, init, cond, update, body);

	return fo;
}
ASTNode* parse_while_stmt(Parser* p) {
	next(p);
	expect_punct(p, "(");
	ASTNode* cond = parse_expr(p);
	expect_punct(p, ")");

	ASTNode* body = parse_stmt(p);

	ASTNode* _while = ast_while_node_create();
	ast_while_node_init(_while, cond, body);
	return _while;
}
ASTNode* parse_if_stmt(Parser* p) {
	next(p);
	ASTNode* if_stmt = ast_if_node_create();
	expect_punct(p, "(");

	ASTNode* cond = parse_expr(p);

	expect_punct(p, ")");

	ASTNode* then = parse_stmt(p);

	ASTNode* else_b = NULL;

	if(match_keywrd(p, "else")) {
		else_b = parse_stmt(p);
	}

	ast_if_node_init(if_stmt, cond, then, else_b);
	return if_stmt;
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
	else if(!strcmp(t->lexeme.data, "var")) {
		return parse_var_decl(p);
	}
	else if(!strcmp(t->lexeme.data, "for")) {
		return parse_for_stmt(p);
	}
	else if(!strcmp(t->lexeme.data, "if")) {
		return parse_if_stmt(p);
	}
	else if(!strcmp(t->lexeme.data, "while")) {
		return parse_while_stmt(p);
	}
	else if(!strcmp(t->lexeme.data, "{")) {
		next(p);
		ASTNode* block = parse_block(p);
		return block;
	}

	ASTNode* expr = parse_expr(p);
	if(!expr) {
		next(p); // Recovery
		return NULL;
	}
	expect_punct(p, ";");
	ASTNode* stmt = ast_expr_stmt_node_create(expr);
	return stmt;
}
ASTNode* parse_expr(Parser* p) {
	return parse_assignment(p);
}

ASTNode* parse_assignment(Parser* p) {
	ASTNode* left = parse_log_or(p);

	if(check_op(p, "=")) {
		next(p);

		ASTNode* value = parse_assignment(p);
		return ast_assign_expr_node_create(left, value);
	}

	return left;
}
ASTNode* parse_log_or(Parser* p) {
	ASTNode* left = parse_log_xor(p);

	while(check_op(p, "||")) {
		Token* op = next(p);
		ASTNode* right = parse_log_xor(p);
		left = ast_binary_expr_node_create(op->lexeme, left, right);
	}

	return left;
}
ASTNode* parse_log_xor(Parser* p) {
	ASTNode* left = parse_log_and(p);

	while(check_op(p, "^^")) {
		Token* op = next(p);
		ASTNode* right = parse_log_and(p);
		left = ast_binary_expr_node_create(op->lexeme, left, right);
	}

	return left;
}
ASTNode* parse_log_and(Parser* p) {
	ASTNode* left = parse_equality(p);

	while(check_op(p, "&&")) {
		Token* op = next(p);
		ASTNode* right = parse_equality(p);
		left = ast_binary_expr_node_create(op->lexeme, left, right);
	}

	return left;
}
ASTNode* parse_equality(Parser* p) {
	ASTNode* left = parse_comparison(p);

	while(check_op(p, "==") || check_op(p, "!=")) {
		Token* op = next(p);
		ASTNode* right = parse_comparison(p);
		left = ast_binary_expr_node_create(op->lexeme, left, right);
	}

	return left;
}
ASTNode* parse_comparison(Parser* p) {
	ASTNode* left = parse_shift(p);

	while(check_op(p, ">") || check_op(p, "<") || check_op(p, ">=") || check_op(p, "<=")) {
		Token* op = next(p);
		ASTNode* right = parse_shift(p);
		left = ast_binary_expr_node_create(op->lexeme, left, right);
	}

	return left;
}
ASTNode* parse_shift(Parser* p) {
	ASTNode* left = parse_term(p);
	while(check_op(p, ">>") || check_op(p, "<<")) {
		Token* op = next(p);
		ASTNode* right = parse_term(p);
		left = ast_binary_expr_node_create(op->lexeme, left, right);
	}
	return left;
}
ASTNode* parse_term(Parser* p) {
	ASTNode* left = parse_factor(p);

	while(check_op(p, "+") || check_op(p, "-")) {
		Token* op = next(p);
		ASTNode* right = parse_factor(p);
		left = ast_binary_expr_node_create(op->lexeme, left, right);
	}

	return left;
}
ASTNode* parse_factor(Parser* p) {
	ASTNode* left = parse_unary(p);

	while(check_op(p, "*") || check_op(p, "/") || check_op(p, "%")) {
		Token* op = next(p);
		ASTNode* right = parse_unary(p);
		left = ast_binary_expr_node_create(op->lexeme, left, right);
	}
	
	return left;
}
ASTNode* parse_unary(Parser* p) {
	if(check_op(p, "-") || check_op(p, "!") || check_op(p, "*") || check_op(p, "&")) {
		Token* op = next(p);
		ASTNode* right = parse_unary(p);
		return ast_unary_expr_node_create(op->lexeme, right);
	}

	return parse_call(p);
}
ASTNode* parse_call(Parser* p) {
	ASTNode* expr = parse_primary(p);

	while(match_punct(p, "(")) {
		expr = parse_call_with_args(p, expr);
	}
	return expr;
}
ASTNode* parse_call_with_args(Parser* p, ASTNode* callee) {
	ASTNode* call = ast_call_expr_node_create();
	ast_call_expr_node_init(call, callee);
	if(!check_punct(p, ")")) {
		do {
			ASTNode* arg = parse_expr(p);
			ast_call_expr_node_add_arg(call, arg);
		} while (match_punct(p, ","));
	}

	expect_punct(p, ")");

	return call;
}
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

	printf("-----SYNTAX ERROR-----\n");
	printf("Expected Expression!\n");
	printf("Got '%s' instead\n", t->lexeme.data);
	printf("Line: %u, Col: %u\n", t->line, t->col);
	printf("----------------------\n");
	return NULL;
}

void parser_free(Parser* p) {
	free(p);
}
