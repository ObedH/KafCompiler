#include "decl_pass.h"

#include <string.h>
#include <stdio.h>

void decl_init(DeclPass* d, Arena* arena) {
	memset(d, 0, sizeof(*d));
	ASTVisitor* visitor = (ASTVisitor*)d;
	d->arena = arena;
	d->symtab = symtab_create(arena, SC_GLOBAL, false);
	visitor->visitFuncDecl = decl_visit_func;
	visitor->visitVarDecl = decl_visit_var;
}
void decl_visit_program(ASTVisitor* visitor, ASTNode* program) {
	DeclPass* d = (DeclPass*)visitor;
	(void)d;

	for(usize i = 0; i < program->prgm.decl_count; i ++) {
		ASTNode* decl = program->prgm.decls[i];
		visit_decl(visitor, decl);
	}
}
void decl_visit_func(ASTVisitor* visitor, ASTNode* node) {
	DeclPass* d = (DeclPass*)visitor;

	const char* name = node->func.name.data;
	if(symtab_lookup(d->symtab, name)) {
		printf("Error: redefinition of function '%s' (line %u, col %u)\n", name, node->line, node->col);
		return;
	}

	Symbol* sym = symbol_create();
	symbol_func_init(sym);
	for(usize i = 0; i < node->func.param_count; i ++) {
		ASTNode* param = node->func.params[i];
		(void)param;
	}
}
void decl_visit_var(ASTVisitor* visitor, ASTNode* node) {
	(void)visitor;
	(void)node;
}
