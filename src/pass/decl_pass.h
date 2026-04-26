#ifndef DECL_PASS_H
#define DECL_PASS_H

#include "../symtab.h"
#include "../arena.h"
#include "../ast.h"

typedef struct {
	SymbolTable* symtab;
	Arena* arena;

	bool verbose;
} DeclPass;
void decl_init(DeclPass* d, Arena* arena, SymbolTable* global_symtab, bool verbose);
void decl_visit_program(DeclPass* d, ASTNode* program);
void decl_visit_var(DeclPass* d, ASTNode* node);
void decl_visit_func(DeclPass* d, ASTNode* node);





#endif
