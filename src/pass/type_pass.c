#include "type_pass.h"

#include <string.h>
#include <stdio.h>

void type_init(TypePass* t, bool verbose, Arena* arena) {
	memset(t, 0, sizeof(*t));
	t->arena = arena;
	t->symtab = symtab_create(arena, SC_GLOBAL, false);
	t->verbose = verbose;
}
void type_visit_program(TypePass* t, ASTNode* program) {

	for(usize i = 0; i < program->prgm.decl_count; i ++) {
		ASTNode* decl = program->prgm.decls[i];
		type_visit_decl(t, decl);
	}
}
void type_visit_decl(TypePass* t, ASTNode* decl) {
	switch(decl->node_type) {
		case NODE_VAR_DECL:
			type_visit_var(t, decl);
			break;
		case NODE_FUNCTION:
			type_visit_func(t, decl);
			break;
		default:
			printf("Expected a delcaration when walking program AST!\n");
			break;
	}
}
void type_visit_var(TypePass* t, ASTNode* node) {
	if(!t) return;
	
	const char* name = node->var_declr.name.data;
	if(symtab_lookup(t->symtab, name)) {
		printf("-----ERROR-----\n");
		printf("Redefinition of variable '%s' (line %u, col %u)\n", name, node->line, node->col);
		printf("---------------\n");
		return;
	}
	Symbol* sym = symbol_create();
	sym->type = SYM_VAR;
	sym->var.node = node;
	sym->var.type = type_visit_type(t, node->var_declr.type);
	sym->var.has_initializer = (node->var_declr.initializer != NULL);
	Type* initializer_type = NULL;
	if(node->var_declr.initializer) {
		initializer_type = type_visit_expr(t, node->var_declr.initializer);
	}
	if(initializer_type != sym->var.type) {
		printf("-----ERROR-----\n");
		printf("Type mismatch of variable '%s' (line %u, col %u)\n", name);
		printf("Initializer:\n");
		type_print(initializer_type, 1);
		printf("Type declaration:\n");
		type_print(sym->var.type, 1);
		printf("---------------\n");
		return;
	}
	sym->var.is_parameter = false;
	symtab_insert(t->symtab, name, sym);
	symbol_print(sym, 0);
}
void type_visit_func(TypePass* t, ASTNode* node) {
	if(!t) return;

	const char* name = node->func.name.data;
	if(symtab_lookup(t->symtab, name)) {
		printf("Error: redefinition of function '%s' (line %u, col %u)\n", name, node->line, node->col);
		return;
	}

	Symbol* sym = symbol_create();
	symbol_func_init(sym);
	sym->func.return_type = type_visit_type(t, node->func.return_type);

	Type* type = type_make_function(sym->func.return_type);

	for(usize i = 0; i < node->func.param_count; i ++) {
		ASTNode* param = node->func.params[i];
		Type* param_type = type_visit_type(t, param->param.type);
		type_function_add_param(type, param_type);
		String name = param->param.name;
		symbol_func_add_param(sym, param_type, name);
	}
	symbol_print(sym, 0);
	symtab_insert(t->symtab, name, sym);

	node->resolved_symbol = sym;
	node->inferred_type = type;

	type_visit_block(t, node->func.body, false);

}
Type* type_visit_type(TypePass* t, ASTNode* node) {
	(void)t;
	Type* type = type_make_primitive(typekind_from_str(node->type.name.data));
	return type;
}
void type_visit_block(TypePass* t, ASTNode* node, bool is_loop) {
	symtab_push(t->arena, t->symtab, SC_BLOCK, is_loop);

	(void)node;
	
	symtab_pop(t->symtab);
}
Type* type_visit_expr(TypePass* t, ASTNode* node) {
	switch(node->node_type) {
		case NODE_ASSIGN_EXPR:
			return type_visit_assign_expr(t, node);
		case NODE_BINARY_EXPR:
			return type_visit_binary_expr(t, node);
		case NODE_UNARY_EXPR:
			return type_visit_unary_expr(t, node);
		case NODE_CALL_EXPR:
			return type_visit_call_expr(t, node);
		case NODE_LITERAL_INT:
			return type_visit_literal_int(t, node);
		case NODE_IDENTIFIER:
			return type_visit_identifier(t, node);
		default:
			printf("Unknown expression type!\n");
			return NULL;
	}
}
Type* type_visit_assign_expr(TypePass* t, ASTNode* node) {
	Type* left_type = type_visit_expr(t, node->assign_expr.target);
	Type* right_type = type_visit_expr(t, node->assign_expr.value);

	if(!type_is_assignable(left_type, right_type)) {
		printf("-----ERROR-----\n");
		printf("Types are not compatible for assignment!\n");
		printf("Target:\n");
		type_print(left_type, 1);
		printf("Value:\n");
		type_print(right_type, 1);
		printf("---------------\n");
		return NULL;
	}
	return right_type;
}
