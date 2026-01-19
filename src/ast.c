#include "ast.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char* binop_str(BinOp binop) {
	switch(binop) {
	case BINOP_PLUS:
		return "PLUS";
	case BINOP_MINUS:
		return "MINUS";
	case BINOP_TIMES:
		return "TIMES";
	case BINOP_DIV:
		return "DIV";
	case BINOP_EQ:
		return "EQUALS";
	case BINOP_NEQ:
		return "NOT EQUALS";
	case BINOP_GT:
		return "GREATER THAN";
	case BINOP_LT:
		return "LESS THAN";
	case BINOP_GE:
		return "GREATER THAN OR EQUAL TO";
	case BINOP_LE:
		return "LESS THAN OR EQUAL TO";
	case BINOP_LOG_AND:
		return "LOGICAL AND";
	case BINOP_LOG_OR:
		return "LOGICAL OR";
	case BINOP_LOG_XOR:
		return "LOGICAL EXCLUSIVE OR";
	default:
		return "UNKNOWN";
	}
}
const char* unop_str(UnOp unop) {
	switch(unop) {
	case UNOP_NEG:
		return "NEGATIVE";
	case UNOP_LOG_NOT:
		return "LOGICAL NOT";
	case UNOP_DEREF:
		return "DEREFERENCE";
	case UNOP_REF:
		return "REFERENCE";
	default:
		return "UNKNOWN";
	}
}

ASTNode* ast_node_create(void) {
	ASTNode* node = malloc(sizeof(ASTNode));
	if(!node) {
		perror("Error mallocating space for ASTNode!");
		return NULL;
	}
	memset(node, 0, sizeof(ASTNode));
	return node;
}
void ast_node_print(ASTNode* ast_node, usize level) {
	switch(ast_node->node_type) {
		case NODE_PROGRAM:
			ast_program_node_print(ast_node, level);
			break;
		default:
			printf("Unknown Node type!\n");
			break;
	}
}
void ast_node_free(ASTNode* ast_node) {
	free(ast_node);
}

ASTNode* ast_program_node_create(void) {
	ASTNode* node = ast_node_create();
	node->node_type = NODE_PROGRAM;
	node->prgm.capacity = 16;
	node->prgm.decls = malloc(sizeof(ASTNode*) * node->prgm.capacity);
	return node;
}
void ast_program_node_append(ASTNode* p, ASTNode* a) {
	if(!p || !a) {
		return;
	}
	if(p->node_type != NODE_PROGRAM) {
		perror("Parent is not a program!");
		return;
	}
	if(a->node_type != NODE_FUNCTION) {
		perror("Child is not a function!");
		// Unimplemented: it could be a variable declaration.
		return;
	}
	if(p->prgm.decl_count == p->prgm.capacity) {
		p->prgm.capacity += 16;
		void* tmp = realloc(p->prgm.decls, sizeof(ASTNode*) * p->prgm.capacity);
		if(!tmp) {
			perror("Error reallocating memory for AST program declarations!");
		}
		else {
			p->prgm.decls = tmp;
		}
	}
	p->prgm.decls[p->prgm.decl_count++] = a;
}
void ast_program_node_print(ASTNode* p, usize level) {
	ptabs(level);
	printf("-----PROGRAM-----\n");
	ptabs(level);
	printf("Declaration count: %u\n", p->prgm.decl_count);
	ptabs(level);
	printf("Declarations:\n");
	for(u16 i = 0; i < p->prgm.decl_count; i ++) {
		if(p->prgm.decls[i]->node_type == NODE_VAR_DECL) {
		}
		if(p->prgm.decls[i]->node_type == NODE_FUNCTION) {
			ast_func_node_print(p->prgm.decls[i], level + 1);
		}
	}
	printf("-----------------\n");
}
void ast_program_node_free(ASTNode* p) {
	// Must free child nodes!
	for(u16 i = 0; i < p->prgm.decl_count; i ++) {
		if(p->prgm.decls[i]->node_type == NODE_VAR_DECL) {
		}
		if(p->prgm.decls[i]->node_type == NODE_FUNCTION) {
			ast_func_node_free(p->prgm.decls[i]);
		}
	}
	free(p->prgm.decls);
	free(p);
}

ASTNode* ast_func_node_create(void) {
	ASTNode* node = ast_node_create();
	node->node_type = NODE_FUNCTION;
	node->func.params = malloc(sizeof(ASTNode*) * 16);
	if(!node->func.params) {
		perror("Error mallocating memory for AST function parameters");
	}
	return node;
}
void ast_func_node_add_param(ASTNode* fn, ASTNode* p) {
	if(!fn || !p) return;
	if(fn->node_type != NODE_FUNCTION) perror("Parent is not a function!");
	if(p->node_type != NODE_PARAM) perror("Child is not a parameter!");

	if(fn->func.param_count == 16) return;
	fn->func.params[fn->func.param_count++] = p;
}
void ast_func_node_init(ASTNode* fn, String name, ASTNode* body, ASTNode* ret_type) {
	fn->func.name = name;
	fn->func.body = body;
	fn->func.return_type = ret_type;
}
void ast_func_node_print(ASTNode* fn, usize l) {
	ptabs(l);
	printf("-----FUNCTION-----\n");

	ptabs(l);
	printf("Name: %s\n", fn->func.name.data);

	ptabs(l);
	printf("Return Type:\n");
	ast_type_node_print(fn->func.return_type, l + 1);

	ptabs(l);
	printf("Params:\n");
	for(u8 i = 0; i < fn->func.param_count; i ++) {
		ast_param_node_print(fn->func.params[i], l + 1);
	}

	ptabs(l);
	printf("Function Body:\n");
	ast_block_node_print(fn->func.body, l + 1);

	ptabs(l);
	printf("------------------\n");
}
void ast_func_node_free(ASTNode* fn) {
	ast_type_node_free(fn->func.return_type);
	ast_block_node_free(fn->func.body);
	for(u8 i = 0; i < fn->func.param_count; i ++) {
		ast_param_node_free(fn->func.params[i]);
	}
	free(fn->func.params);
	free(fn);
}
ASTNode* ast_type_node_create(void) {
	ASTNode* node = ast_node_create();
	node->node_type = NODE_TYPE_NAME;
	return node;
}
void ast_type_node_init(ASTNode* t, String name) {
	if(!t || t->node_type != NODE_TYPE_NAME) {
		return;
	}
	t->type.name = name;
}
void ast_type_node_print(ASTNode* t, usize l) {
	ptabs(l);
	printf("-----TYPE-----\n");

	ptabs(l);
	printf("Name: %s\n", t->type.name.data);

	ptabs(l);
	printf("--------------\n");
}
void ast_type_node_free(ASTNode* t) {
	free(t);
}

ASTNode* ast_param_node_create(void) {
	ASTNode* node = ast_node_create();
	node->node_type = NODE_PARAM;
	return node;
}
void ast_param_node_init(ASTNode* t, String name, ASTNode* type) {
	t->param.name = name;
	t->param.type = type;
}
void ast_param_node_print(ASTNode* t, usize l) {
	ptabs(l);
	printf("-----PARAMETER-----\n");

	ptabs(l);
	printf("Name: %s\n", t->param.name.data);

	ptabs(l);
	printf("Type:\n");
	ast_type_node_print(t->param.type, l + 1);

	ptabs(l);
	printf("-------------------\n");
}
void ast_param_node_free(ASTNode* t) {
	ast_type_node_free(t->param.type);
	free(t);
}

ASTNode* ast_block_node_create(void) {
	ASTNode* node = ast_node_create();
	node->node_type = NODE_BLOCK;
	node->block.capacity = 16;
	node->block.stmts = malloc(sizeof(ASTNode*) * node->block.capacity);
	if(!node->block.stmts) {
		perror("Failed to mallocate memory for statements!");
	}
	return node;
}
void ast_block_node_add_stmt(ASTNode* b, ASTNode* s) {
	if(!b) {
		perror("Parent is null!");
		return;
	}
	if(!s) {
		perror("Child is null!");
	}
	if(b->node_type != NODE_BLOCK) {
		perror("Parent not a block!");
		return;
	}
	if(b->block.stmt_count == b->block.capacity) {
		b->block.capacity += 16;
		void* tmp = realloc(b->block.stmts, sizeof(ASTNode*) * b->block.capacity);
		if(!tmp) {
			perror("Failed to reallocate memory for a new statement!");
		}
		else {
			b->block.stmts = tmp;
		}
	}
	b->block.stmts[b->block.stmt_count++] = s;
}
void ast_block_node_print(ASTNode* b, usize level) {
	ptabs(level);
	printf("-----BLOCK-----\n");

	ptabs(level);
	printf("Statement Count: %u\n", b->block.stmt_count);

	ptabs(level);
	printf("Statements:\n");
	for(u16 i = 0; i < b->block.stmt_count; i ++) {
		ast_stmt_node_print(b->block.stmts[i], level + 1);
	}

	ptabs(level);
	printf("---------------\n");
}
void ast_block_node_free(ASTNode* b) {
	for(u16 i = 0; i < b->block.stmt_count; i ++) {
		ast_stmt_node_free(b->block.stmts[i]);
	}
	free(b->block.stmts);
	free(b);
}

void ast_stmt_node_print(ASTNode* s, usize l) {
	if(s->node_type == NODE_RETURN_STMT) {
		ast_return_node_print(s, l);
	}
}
void ast_stmt_node_free(ASTNode* s) {
	if(s->node_type == NODE_RETURN_STMT) {
		ast_return_node_free(s);
	}
}

ASTNode* ast_return_node_create(void) {
	ASTNode* node = ast_node_create();
	node->node_type = NODE_RETURN_STMT;
	return node;
}
void ast_return_node_init(ASTNode* r, ASTNode* e) {
	r->return_stmt.expr = e;
}
void ast_return_node_print(ASTNode* r, usize l) {
	ptabs(l);
	printf("-----RETURN STATEMENT-----\n");

	if(r->return_stmt.expr) {
		ptabs(l);
		printf("Return expression:\n");
		ast_expr_node_print(r->return_stmt.expr, l + 1);
	}

	ptabs(l);
	printf("--------------------------\n");
}
void ast_return_node_free(ASTNode* r) {
	if(r->return_stmt.expr) {
		ast_expr_node_free(r->return_stmt.expr);
	}
	free(r);
}

ASTNode* ast_assign_expr_node_create(ASTNode* left, ASTNode* value) {
	ASTNode* a = ast_node_create();
	a->node_type = NODE_ASSIGN_EXPR;
	a->assign_expr.target = left;
	a->assign_expr.value = value;
	return a;
}
void ast_assign_expr_node_print(ASTNode* a, usize l) {
	ptabs(l);
	printf("-----ASSIGNMENT EXPRESSION-----\n");

	ptabs(l);
	printf("Target:\n");
	ast_expr_node_print(a->assign_expr.target, l + 1);

	ptabs(l);
	printf("Value:\n");
	ast_expr_node_print(a->assign_expr.value, l + 1);

	ptabs(l);
	printf("-------------------------------\n");
}
void ast_assign_expr_node_free(ASTNode* a) {
	ast_expr_node_free(a->assign_expr.target);
	ast_expr_node_free(a->assign_expr.value);
	free(a);
}

void ast_expr_node_print(ASTNode* i, usize l) {
	switch(i->node_type) {
	case NODE_LITERAL_INT:
		ast_literal_int_node_print(i, l);
		break;
	case NODE_ASSIGN_EXPR:
		ast_assign_expr_node_print(i, l);
		break;
	case NODE_IDENTIFIER:
		ast_identifier_node_print(i, l);
		break;
	case NODE_BINARY_EXPR:
		ast_binary_expr_node_print(i, l);
		break;
	case NODE_UNARY_EXPR:
		ast_unary_expr_node_print(i, l);
		break;
	default:
		perror("Unhandled expression type!");
	}
}
void ast_expr_node_free(ASTNode* i) {
	switch(i->node_type) {
	case NODE_LITERAL_INT:
		ast_literal_int_node_free(i);
		break;
	case NODE_ASSIGN_EXPR:
		ast_assign_expr_node_free(i);
		break;
	case NODE_IDENTIFIER:
		ast_identifier_node_free(i);
		break;
	case NODE_BINARY_EXPR:
		ast_binary_expr_node_free(i);
		break;
	case NODE_UNARY_EXPR:
		ast_unary_expr_node_free(i);
		break;
	default:
		perror("Unhandled expression type!");
	}
}

ASTNode* ast_literal_int_node_create(usize int_value) {
	ASTNode* node = ast_node_create();
	node->node_type = NODE_LITERAL_INT;
	node->literal_int.value = int_value;
	return node;
}
void ast_literal_int_node_print(ASTNode* i, usize l) {
	ptabs(l);
	printf("-----INT LITERAL-----\n");

	ptabs(l);
	printf("Value: %u\n", i->literal_int.value);

	ptabs(l);
	printf("---------------------\n");
}
void ast_literal_int_node_free(ASTNode* i) {
	free(i);
}

ASTNode* ast_identifier_node_create(String name) {
	ASTNode* node = ast_node_create();
	node->node_type = NODE_IDENTIFIER;
	node->identifier.name = name;
	return node;
}
void ast_identifier_node_print(ASTNode* i, usize l) {
	ptabs(l);
	printf("-----IDENTIFIER-----\n");

	ptabs(l);
	printf("Name: %s\n", i->identifier.name.data);

	ptabs(l);
	printf("--------------------\n");
}
void ast_identifier_node_free(ASTNode* i) {
	free(i);
}

ASTNode* ast_binary_expr_node_create(String lexeme, ASTNode* left, ASTNode* right) {
	ASTNode* node = ast_node_create();
	node->node_type = NODE_BINARY_EXPR;

	if(!strcmp(lexeme.data, "+")) {
		node->binary_expr.op = BINOP_PLUS;
	}
	else if(!strcmp(lexeme.data, "-")) {
		node->binary_expr.op = BINOP_MINUS;
	}
	else if(!strcmp(lexeme.data, "*")) {
		node->binary_expr.op = BINOP_TIMES;
	}
	else if(!strcmp(lexeme.data, "/")) {
		node->binary_expr.op = BINOP_DIV;
	}
	else if(!strcmp(lexeme.data, "==")) {
		node->binary_expr.op = BINOP_EQ;
	}
	else if(!strcmp(lexeme.data, "!=")) {
		node->binary_expr.op = BINOP_NEQ;
	}
	else if(!strcmp(lexeme.data, ">")) {
		node->binary_expr.op = BINOP_GT;
	}
	else if(!strcmp(lexeme.data, "<")) {
		node->binary_expr.op = BINOP_LT;
	}
	else if(!strcmp(lexeme.data, ">=")) {
		node->binary_expr.op = BINOP_GE;
	}
	else if(!strcmp(lexeme.data, "<=")) {
		node->binary_expr.op = BINOP_LE;
	}
	else if(!strcmp(lexeme.data, "&&")) {
		node->binary_expr.op = BINOP_LOG_AND;
	}
	else if(!strcmp(lexeme.data, "||")) {
		node->binary_expr.op = BINOP_LOG_OR;
	}
	else if(!strcmp(lexeme.data, "^^")) {
		node->binary_expr.op = BINOP_LOG_XOR;
	}

	node->binary_expr.left = left;
	node->binary_expr.right = right;
	return node;
}
void ast_binary_expr_node_print(ASTNode* b, usize l) {
	ptabs(l);
	printf("-----BINARY EXPRESSION-----\n");

	ptabs(l);
	printf("Operation: %s\n", binop_str(b->binary_expr.op));

	ptabs(l);
	printf("Left: \n");
	ast_expr_node_print(b->binary_expr.left, l + 1);

	ptabs(l);
	printf("Right: \n");
	ast_expr_node_print(b->binary_expr.right, l + 1);

	ptabs(l);
	printf("---------------------------\n");
}
void ast_binary_expr_node_free(ASTNode* b) {
	ast_expr_node_free(b->binary_expr.left);
	ast_expr_node_free(b->binary_expr.right);
	free(b);
}

ASTNode* ast_unary_expr_node_create(String lexeme, ASTNode* operand) {
	ASTNode* node = ast_node_create();
	node->node_type = NODE_UNARY_EXPR;

	if(!strcmp(lexeme.data, "-")) {
		node->unary_expr.op = UNOP_NEG;
	}
	else if(!strcmp(lexeme.data, "!")) {
		node->unary_expr.op = UNOP_LOG_NOT;
	}
	else if(!strcmp(lexeme.data, "*")) {
		node->unary_expr.op = UNOP_DEREF;
	}
	else if(!strcmp(lexeme.data, "&")) {
		node->unary_expr.op = UNOP_REF;
	}

	node->unary_expr.operand = operand;
	return node;
}
void ast_unary_expr_node_print(ASTNode* u, usize l) {
	ptabs(l);
	printf("-----UNARY EXPRESSION-----\n");

	ptabs(l);
	printf("Operation: %s\n", unop_str(u->unary_expr.op));
	ptabs(l);
	printf("Operand:\n");
	ast_expr_node_print(u->unary_expr.operand, l + 1);


	ptabs(l);
	printf("--------------------------\n");
}
void ast_unary_expr_node_free(ASTNode* u) {
	ast_expr_node_free(u->unary_expr.operand);
	free(u);
}
