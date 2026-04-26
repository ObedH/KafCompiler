#include "ast.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* -------------------- ENUM TO STRING HELPER FUNCTIONS -------------------- */

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
	case BINOP_BIT_AND:
		return "BITWISE AND";
	case BINOP_BIT_OR:
		return "BITWISE OR";
	case BINOP_BIT_XOR:
		return "BITWISE EXCLUSIVE OR";
	case BINOP_LSHIFT:
		return "LEFT BIT SHIFT";
	case BINOP_RSHIFT:
		return "RIGHT BIT SHIFT";
	case BINOP_MOD:
		return "MODULO";
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
	case UNOP_BIT_NOT:
		return "BITWISE NOT";
	default:
		return "UNKNOWN";
	}
}

/* -------------------- GENERIC NODE HELPER FUNCTIONS -------------------- */

ASTNode* ast_node_create(void) {
	ASTNode* node = malloc(sizeof(ASTNode));
	if(!node) {
		perror("Error mallocating space for ASTNode!");
		return NULL;
	}
	memset(node, 0, sizeof(ASTNode));
	return node;
}
void ast_node_print(ASTNode* ast_node, usize l) {
	if(!ast_node) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
	switch(ast_node->node_type) {
		case NODE_PROGRAM:
			ast_program_node_print(ast_node, l);
			break;
		default:
			ptabs(l);
			printf("Unknown Node type!\n");
			break;
	}
}
void ast_node_free(ASTNode* ast_node) {
	free(ast_node);
}
static void free_ast(ASTNode* ast_node) {
	if(ast_node) {
		if(ast_node->inferred_type) type_free(ast_node->inferred_type);
	}
	free(ast_node);
}

/* -------------------- PROGRAM NODE -------------------- */

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
	if(a->node_type != NODE_FUNCTION && a->node_type != NODE_VAR_DECL) {
		perror("Child is not a function or variable declaration!");
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
void ast_program_node_print(ASTNode* p, usize l) {
	if(!p) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
	ptabs(l);
	printf("-----PROGRAM-----\n");
	ptabs(l);
	printf("Declaration count: %u\n", p->prgm.decl_count);
	ptabs(l);
	printf("Declarations:\n");
	for(u16 i = 0; i < p->prgm.decl_count; i ++) {
		if(p->prgm.decls[i]->node_type == NODE_VAR_DECL) {
			ast_var_node_print(p->prgm.decls[i], l + 1);
		}
		if(p->prgm.decls[i]->node_type == NODE_FUNCTION) {
			ast_func_node_print(p->prgm.decls[i], l + 1);
		}
	}
	printf("-----------------\n");
}
void ast_program_node_free(ASTNode* p) {
	// Must free child nodes!
	if(p->prgm.decls) {
		for(u16 i = 0; i < p->prgm.decl_count; i ++) {
			if(!p->prgm.decls[i]) {
				continue;
			}
			if(p->prgm.decls[i]->node_type == NODE_VAR_DECL) {
				ast_var_node_free(p->prgm.decls[i]);
			}
			else if(p->prgm.decls[i]->node_type == NODE_FUNCTION) {
				ast_func_node_free(p->prgm.decls[i]);
			}
		}
	}
	free(p->prgm.decls);
	free_ast(p);
}

/* -------------------- FUNCTION DECL NODE -------------------- */

ASTNode* ast_func_node_create(void) {
	ASTNode* node = ast_node_create();
	if(!node) {
		return NULL;
	}
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
fn->func.name = string_dup(name);
	fn->func.body = body;
	fn->func.return_type = ret_type;
}
void ast_func_node_print(ASTNode* fn, usize l) {
	if(!fn) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
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
	string_free(fn->func.name);
	free(fn->func.params);
	free_ast(fn);
}

/* -------------------- VAR DECL NODE -------------------- */

ASTNode* ast_var_node_create(void) {
	ASTNode* node = ast_node_create();
	node->node_type = NODE_VAR_DECL;
	return node;
}
void ast_var_node_init(ASTNode* node, String name, ASTNode* type, ASTNode* initializer) {
	if(!node || node->node_type != NODE_VAR_DECL) {
		return;
	}
	node->var_declr.name = string_dup(name);
	node->var_declr.type = type;
	node->var_declr.initializer = initializer;
}
void ast_var_node_print(ASTNode* node, usize l) {
	if(!node) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
	ptabs(l);
	printf("-----VAR-----\n");

	ptabs(l);
	printf("Name: %s\n", node->var_declr.name.data);

	ptabs(l);
	printf("Type:\n");
	ast_type_node_print(node->var_declr.type, l + 1);
	
	if(node->var_declr.initializer) {
		ptabs(l);
		printf("Initializer:\n");
		ast_expr_node_print(node->var_declr.initializer, l + 1);
	}
	else {
		ptabs(l);
		printf("Initializer: (none)\n");
	}

	ptabs(l);
	printf("-------------\n");
}
void ast_var_node_free(ASTNode* node) {
	string_free(node->var_declr.name);
	ast_type_node_free(node->var_declr.type);
	ast_expr_node_free(node->var_declr.initializer);
	free_ast(node);
}

/* -------------------- TYPE NODE -------------------- */

ASTNode* ast_type_node_create(void) {
	ASTNode* node = ast_node_create();
	node->node_type = NODE_TYPE_NAME;
	return node;
}
void ast_type_node_init(ASTNode* t, String name) {
	if(!t || t->node_type != NODE_TYPE_NAME) {
		return;
	}
	t->type.name = string_dup(name);
}
void ast_type_node_print(ASTNode* t, usize l) {
	if(!t) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
	ptabs(l);
	printf("-----TYPE-----\n");

	ptabs(l);
	printf("Name: %s\n", t->type.name.data);

	ptabs(l);
	printf("--------------\n");
}
void ast_type_node_free(ASTNode* t) {
	string_free(t->type.name);
	free_ast(t);
}

/* -------------------- PARAMETER NODE -------------------- */

ASTNode* ast_param_node_create(void) {
	ASTNode* node = ast_node_create();
	node->node_type = NODE_PARAM;
	return node;
}
void ast_param_node_init(ASTNode* t, String name, ASTNode* type) {
	t->param.name = string_dup(name);
	t->param.type = type;
}
void ast_param_node_print(ASTNode* t, usize l) {
	if(!t) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
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
	string_free(t->param.name);
	free_ast(t);
}

/* -------------------- BLOCK NODE -------------------- */

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
void ast_block_node_print(ASTNode* b, usize l) {
	if(!b) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
	ptabs(l);
	printf("-----BLOCK-----\n");

	ptabs(l);
	printf("Statement Count: %u\n", b->block.stmt_count);

	ptabs(l);
	printf("Statements:\n");
	for(u16 i = 0; i < b->block.stmt_count; i ++) {
		ast_stmt_node_print(b->block.stmts[i], l + 1);
	}

	ptabs(l);
	printf("---------------\n");
}
void ast_block_node_free(ASTNode* b) {
	for(u16 i = 0; i < b->block.stmt_count; i ++) {
		ast_stmt_node_free(b->block.stmts[i]);
	}
	free(b->block.stmts);
	free_ast(b);
}

/* -------------------- STATEMENT DISPATCHERS -------------------- */

void ast_stmt_node_print(ASTNode* s, usize l) {
	if(!s) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
	switch(s->node_type) {
		case NODE_RETURN_STMT:
			ast_return_node_print(s, l);
			break;
		case NODE_EXPR_STMT:
			ast_expr_stmt_node_print(s, l);
			break;
		case NODE_FOR_STMT:
			ast_for_node_print(s, l);
			break;
		case NODE_WHILE_STMT:
			ast_while_node_print(s, l);
			break;
		case NODE_IF_STMT:
			ast_if_node_print(s, l);
			break;
		case NODE_BLOCK:
			ast_block_node_print(s, l);
			break;
		case NODE_VAR_DECL:
			ast_var_node_print(s, l);
			break;
		default:
			ptabs(l);
			printf("Unknown statement type! (Trying to print %u)\n", s->node_type);
			break;
	}
}
void ast_stmt_node_free(ASTNode* s) {
	switch(s->node_type) {
		case NODE_RETURN_STMT:
			ast_return_node_free(s);
			break;
		case NODE_EXPR_STMT:
			ast_expr_stmt_node_free(s);
			break;
		case NODE_FOR_STMT:
			ast_for_node_free(s);
			break;
		case NODE_WHILE_STMT:
			ast_while_node_free(s);
			break;
		case NODE_IF_STMT:
			ast_if_node_free(s);
			break;
		case NODE_BLOCK:
			ast_block_node_free(s);
			break;
		case NODE_VAR_DECL:
			ast_var_node_free(s);
			break;
		default:
			printf("Unknown statement type! (Trying to free %u)\n", s->node_type);
			break;
	}
}

/* -------------------- RETURN STATEMENT -------------------- */

ASTNode* ast_return_node_create(void) {
	ASTNode* node = ast_node_create();
	node->node_type = NODE_RETURN_STMT;
	return node;
}
void ast_return_node_init(ASTNode* r, ASTNode* e) {
	r->return_stmt.expr = e;
}
void ast_return_node_print(ASTNode* r, usize l) {
	if(!r) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
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
	free_ast(r);
}

/* -------------------- EXPRESSION STATEMENT -------------------- */

ASTNode* ast_expr_stmt_node_create(ASTNode* expr) {
	ASTNode* node = ast_node_create();
	node->node_type = NODE_EXPR_STMT;
	node->expr_stmt.expr = expr;
	return node;
}
void ast_expr_stmt_node_print(ASTNode* es, usize l) {
	if(!es) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
	ptabs(l);
	printf("-----EXPR STMT-----\n");

	ptabs(l);
	printf("Expression:\n");
	ast_expr_node_print(es->expr_stmt.expr, l + 1);

	ptabs(l);
	printf("-------------------\n");
}
void ast_expr_stmt_node_free(ASTNode* es) {
	ast_expr_node_free(es->expr_stmt.expr);
	free_ast(es);
}

/* -------------------- FOR LOOP STATEMENT -------------------- */

ASTNode* ast_for_node_create(void) {
	ASTNode* tmp = ast_node_create();
	tmp->node_type = NODE_FOR_STMT;
	return tmp;
}
void ast_for_node_init(ASTNode* f, ASTNode* i, ASTNode* c, ASTNode* u, ASTNode* s) {
	if(!f) {
		return;
	}
	if(i) f->for_stmt.init = i;
	if(c) f->for_stmt.cond = c;
	if(u) f->for_stmt.update = u;
	if(s) f->for_stmt.stmt = s;
}
void ast_for_node_print(ASTNode* f, usize l) {
	if(!f) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
	ptabs(l);
	printf("-----FOR LOOP-----\n");

	if(f->for_stmt.init) {
		ptabs(l);
		printf("Initializer:\n");
		if(f->for_stmt.init->node_type == NODE_VAR_DECL) {
			ast_var_node_print(f->for_stmt.init, l + 1);
		}
		else if(f->for_stmt.init->node_type == NODE_EXPR_STMT) {
			ast_expr_stmt_node_print(f->for_stmt.init, l + 1);
		}
		else {
			perror("Invalid for loop initializer!");
		}
	}
	else {
		ptabs(l);
		printf("Initializer: (none)\n");
	}

	if(f->for_stmt.cond) {
		ptabs(l);
		printf("Condition:\n");
		ast_expr_node_print(f->for_stmt.cond, l + 1);
	}
	else {
		ptabs(l);
		printf("Condition: (none)\n");
	}

	if(f->for_stmt.update) {
		ptabs(l);
		printf("Update:\n");
		ast_expr_node_print(f->for_stmt.update, l + 1);
	}
	else {
		ptabs(l);
		printf("Update: (none)\n");
	}

	if(f->for_stmt.stmt) {
		ptabs(l);
		printf("Body:\n");
		ast_stmt_node_print(f->for_stmt.stmt, l + 1);
	}
	else {
		ptabs(l);
		printf("Body: (none)\n");
	}

	ptabs(l);
	printf("------------------\n");
}
void ast_for_node_free(ASTNode* f) {
	if(f) {
		if(f->for_stmt.init) {
			if(f->for_stmt.init->node_type == NODE_VAR_DECL) {
				ast_var_node_free(f->for_stmt.init);
			}
			else if(f->for_stmt.init->node_type == NODE_EXPR_STMT) {
				ast_expr_stmt_node_free(f->for_stmt.init);
			}
			else {
				perror("Invalid for loop initializer!");
			}
		}
		ast_expr_node_free(f->for_stmt.cond);
		ast_expr_node_free(f->for_stmt.update);
		ast_stmt_node_free(f->for_stmt.stmt);
	}
	free_ast(f);
}

/* -------------------- WHILE LOOP STATEMENT -------------------- */

ASTNode* ast_while_node_create(void) {
	ASTNode* tmp = ast_node_create();
	tmp->node_type = NODE_WHILE_STMT;
	return tmp;
}
void ast_while_node_init(ASTNode* w, ASTNode* c, ASTNode* b) {
	if(!w) return;
	w->while_stmt.cond = c;
	w->while_stmt.body = b;
}
void ast_while_node_print(ASTNode* w, usize l) {
	if(!w) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
	ptabs(l);
	printf("-----WHILE LOOP-----\n");
	
	if(w->while_stmt.cond) {
		ptabs(l);
		printf("Condition:\n");
		ast_expr_node_print(w->while_stmt.cond, l + 1);
	}
	else {
		ptabs(l);
		printf("Condition: (none)\n");
	}

	if(w->while_stmt.body) {
		ptabs(l);
		printf("Body:\n");
		ast_stmt_node_print(w->while_stmt.body, l + 1);
	}
	else {
		ptabs(l);
		printf("Body: (none)\n");
	}

	ptabs(l);
	printf("--------------------\n");
}
void ast_while_node_free(ASTNode* w) {
	if(w) {
		if(w->while_stmt.cond) ast_expr_node_free(w->while_stmt.cond);
		if(w->while_stmt.body) ast_stmt_node_free(w->while_stmt.body);
	}
	free_ast(w);
}

/* -------------------- IF STATEMENT -------------------- */

ASTNode* ast_if_node_create(void) {
	ASTNode* tmp = ast_node_create();
	tmp->node_type = NODE_IF_STMT;
	return tmp;
}
void ast_if_node_init(ASTNode* i, ASTNode* c, ASTNode* t, ASTNode* e) {
	if(!i) return;
	if(c) {
		i->if_stmt.cond = c;
	}
	if(t) {
		i->if_stmt.then_branch = t;
	}
	if(e) {
		i->if_stmt.else_branch = e;
	}
}
void ast_if_node_print(ASTNode* i, usize l) {
	if(!i) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
	ptabs(l);
	printf("-----IF STATEMENT-----\n");

	if(i->if_stmt.cond) {
		ptabs(l);
		printf("Condition:\n");
		ast_expr_node_print(i->if_stmt.cond, l + 1);
	}
	else {
		ptabs(l);
		printf("Condition: (none)\n");
	}

	if(i->if_stmt.then_branch) {
		ptabs(l);
		printf("Then:\n");
		ast_stmt_node_print(i->if_stmt.then_branch, l + 1);
	}
	else {
		ptabs(l);
		printf("Then: (none)\n");
	}

	if(i->if_stmt.else_branch) {
		ptabs(l);
		printf("Else:\n");
		ast_stmt_node_print(i->if_stmt.else_branch, l + 1);
	}
	else {
		ptabs(l);
		printf("Else: (none)\n");
	}

	ptabs(l);
	printf("----------------------\n");
}
void ast_if_node_free(ASTNode* i) {
	if(i) {
		if(i->if_stmt.cond) {
			ast_expr_node_free(i->if_stmt.cond);
		}
		if(i->if_stmt.then_branch) {
			ast_stmt_node_free(i->if_stmt.then_branch);
		}
		if(i->if_stmt.else_branch) {
			ast_stmt_node_free(i->if_stmt.else_branch);
		}
	}
	free_ast(i);
}

/* -------------------- EXPRESSION DISPATCHERS -------------------- */

void ast_expr_node_print(ASTNode* i, usize l) {
	if(!i) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
	switch(i->node_type) {
	case NODE_LITERAL:
		ast_literal_node_print(i, l);
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
	case NODE_CALL_EXPR:
		ast_call_expr_node_print(i, l);
		break;
	default:
		perror("Unhandled expression type!");
	}
}
void ast_expr_node_free(ASTNode* i) {
	if(!i) {
		return;
	}
	switch(i->node_type) {
	case NODE_ASSIGN_EXPR:
		ast_assign_expr_node_free(i);
		break;
	case NODE_LITERAL:
		ast_literal_node_free(i);
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
	case NODE_CALL_EXPR:
		ast_call_expr_node_free(i);
		break;
	default:
		perror("Unhandled expression type!");
	}
}

/* -------------------- ASSIGNMENT EXPRESSION -------------------- */

ASTNode* ast_assign_expr_node_create(ASTNode* left, ASTNode* value) {
	ASTNode* a = ast_node_create();
	a->node_type = NODE_ASSIGN_EXPR;
	a->assign_expr.target = left;
	a->assign_expr.value = value;
	return a;
}
void ast_assign_expr_node_print(ASTNode* a, usize l) {
	if(!a) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
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

/* -------------------- LITERAL EXPRESSION -------------------- */

ASTNode* ast_literal_node_create(Literal lit) {
	ASTNode* node = ast_node_create();
	node->node_type = NODE_LITERAL;
	node->literal.lit = lit;
	return node;
}
void ast_literal_node_print(ASTNode* i, usize l) {
	if(!i) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
	ptabs(l);
	printf("-----LITERAL-----\n");

	ptabs(l);
	printf("Value: %d\n", (int)i->literal.lit.int_value);

	ptabs(l);
	printf("---------------------\n");
}
void ast_literal_node_free(ASTNode* i) {
	free_ast(i);
}

/* -------------------- IDENTIFIER EXPRESSION -------------------- */

ASTNode* ast_identifier_node_create(String name) {
	ASTNode* node = ast_node_create();
	node->node_type = NODE_IDENTIFIER;
	node->identifier.name = string_dup(name);
	return node;
}
void ast_identifier_node_print(ASTNode* i, usize l) {
	if(!i) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
	ptabs(l);
	printf("-----IDENTIFIER-----\n");

	ptabs(l);
	printf("Name: %s\n", i->identifier.name.data);

	ptabs(l);
	printf("--------------------\n");
}
void ast_identifier_node_free(ASTNode* i) {
	string_free(i->identifier.name);
	free_ast(i);
}

/* -------------------- BINARY EXPRESSION -------------------- */

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
	else if(!strcmp(lexeme.data, "&")) {
		node->binary_expr.op = BINOP_BIT_AND;
	}
	else if(!strcmp(lexeme.data, "|")) {
		node->binary_expr.op = BINOP_BIT_OR;
	}
	else if(!strcmp(lexeme.data, "^")) {
		node->binary_expr.op = BINOP_BIT_XOR;
	}
	else if(!strcmp(lexeme.data, "<<")) {
		node->binary_expr.op = BINOP_LSHIFT;
	}
	else if(!strcmp(lexeme.data, ">>")) {
		node->binary_expr.op = BINOP_RSHIFT;
	}
	else if(!strcmp(lexeme.data, "%")) {
		node->binary_expr.op = BINOP_MOD;
	}

	node->binary_expr.left = left;
	node->binary_expr.right = right;
	return node;
}
void ast_binary_expr_node_print(ASTNode* b, usize l) {
	if(!b) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
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
	free_ast(b);
}

/* -------------------- UNARY EXPRESSION -------------------- */

ASTNode* ast_unary_expr_node_create(String lexeme, ASTNode* operand) {
	ASTNode* node = ast_node_create();
	node->node_type = NODE_UNARY_EXPR;

	if(!strcmp(lexeme.data, "-")) {
		node->unary_expr.op = UNOP_NEG;
	}
	else if(!strcmp(lexeme.data, "!")) {
		node->unary_expr.op = UNOP_LOG_NOT;
	}
	else if(!strcmp(lexeme.data, "~")) {
		node->unary_expr.op = UNOP_BIT_NOT;
	}

	node->unary_expr.operand = operand;
	return node;
}
void ast_unary_expr_node_print(ASTNode* u, usize l) {
	if(!u) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
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
	free_ast(u);
}

/* -------------------- CALL EXPRESSION -------------------- */

ASTNode* ast_call_expr_node_create(void) {
	ASTNode* node = ast_node_create();
	node->node_type = NODE_CALL_EXPR;
	node->call_expr.args = malloc(sizeof(ASTNode*) * 16);
	if(!node->call_expr.args) {
		perror("Error mallocating memory for AST function call arguments");
	}
	node->call_expr.arg_count = 0;
	return node;
}
void ast_call_expr_node_init(ASTNode* c, ASTNode* callee) {
	c->call_expr.callee = callee;
}
void ast_call_expr_node_add_arg(ASTNode* c, ASTNode* arg) {
	if(c->call_expr.arg_count == 16) {
		perror("Too many arguments!");
		return;
	}
	c->call_expr.args[c->call_expr.arg_count++] = arg;
}
void ast_call_expr_node_print(ASTNode* c, usize l) {
	if(!c) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
	ptabs(l);
	printf("-----CALL EXPR-----\n");

	ptabs(l);
	printf("Callee:\n");
	ast_expr_node_print(c->call_expr.callee, l + 1);

	ptabs(l);
	printf("Args:\n");
	for(usize i = 0; i < c->call_expr.arg_count; i ++) {
		ast_expr_node_print(c->call_expr.args[i], l + 1);
	}

	ptabs(l);
	printf("-------------------\n");
}
void ast_call_expr_node_free(ASTNode* c) {
	ast_expr_node_free(c->call_expr.callee);
	for(usize i = 0; i < c->call_expr.arg_count; i ++) {
		ast_expr_node_free(c->call_expr.args[i]);
	}
	free(c->call_expr.args);
	free_ast(c);
}
