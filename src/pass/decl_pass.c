#include "decl_pass.h"
#include "../type_resolver.h"

#include <string.h>
#include <stdio.h>

void decl_init(DeclPass* d, Arena* arena, SymbolTable* global_symtab, bool verbose) {
	memset(d, 0, sizeof(*d));
	d->arena = arena;
	d->symtab = global_symtab;
	d->verbose = verbose;
}
void decl_visit_program(DeclPass* d, ASTNode* program) {

	if(d->verbose) printf("-----DECL PASS-----\n");

	for(usize i = 0; i < program->prgm.decl_count; i ++) {
		ASTNode* decl = program->prgm.decls[i];
		switch(decl->node_type) {
			case NODE_FUNCTION:
				decl_visit_func(d, decl);
				break;
			case NODE_VAR_DECL:
				decl_visit_var(d, decl);
				break;
			default:
				printf("Unkown declaration type!\n");
				break;
		}
	}

	if(d->verbose) printf("-------------------\n");
}
void decl_visit_func(DeclPass* d, ASTNode* node) {

	const char* name = node->func.name.data;
	if(symtab_lookup(d->symtab, name)) {
		printf("-----ERROR-----\n");
		printf("Error: redefinition of function '%s' (line %u, col %u)\n", name, node->line, node->col);
		printf("---------------\n");
		return;
	}

	Symbol* sym = symbol_create();
	symbol_func_init(sym);

	Type* return_type = resolve_type(node->func.return_type);
	Type* signature = type_make_function(return_type);

	for(usize i = 0; i < node->func.param_count; i ++) {
		ASTNode* param = node->func.params[i];
		String name = param->param.name;
		Type* param_type = resolve_type(param->param.type);
		type_function_add_param(signature, param_type);
		symbol_func_add_param(sym, param_type, name);
	}

	sym->func.return_type = return_type;
	sym->resolved_type = signature;
	
	if(d->verbose) symbol_print(sym, 1);
	symtab_insert(d->symtab, name, sym);

	node->resolved_symbol = sym;
	node->inferred_type = signature;
}
void decl_visit_var(DeclPass* d, ASTNode* node) {
	if(!d || !node) return;

	const char* name = node->var_declr.name.data;
	if(symtab_lookup(d->symtab, name)) {
		printf("-----ERROR-----\n");
		printf("Redefinition of variable '%s' (line %u, col %u)\n", name, node->line, node->col);
		printf("---------------\n");
		return;
	}

	Symbol* sym = symbol_create();
	sym->type = SYM_VAR;
	sym->var.node = node;
	node->resolved_symbol = sym;
	sym->var.is_parameter = false;

	Type* variable_type = resolve_type(node->var_declr.type);
	sym->var.type = variable_type;
	sym->resolved_type = variable_type;

	symtab_insert(d->symtab, name, sym);
	if(d->verbose) symbol_print(sym, 1);
}
