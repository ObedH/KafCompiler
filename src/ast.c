#include "ast.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
	for(u16 i = 0; i < p->prgm.decl_count; i ++) {
		if(p->prgm.decls[i]->node_type == NODE_VAR_DECL) {
		}
		if(p->prgm.decls[i]->node_type == NODE_FUNCTION) {
			ast_func_node_print(p->prgm.decls[i], level + 1);
		}
	}
	ptabs(level);
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
	//ast_compound_stmt_print(fn->func.body, l + 1);

	ptabs(l);
	printf("------------------\n");
}
void ast_func_node_free(ASTNode* fn) {
	ast_type_node_free(fn->func.return_type);
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
