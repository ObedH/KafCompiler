#ifndef DECL_PASS_H
#define DECL_PASS_H

#include "../visitor.h"
#include "../symtab.h"
#include "../arena.h"
#include "../ast.h"

typedef struct {
	ASTVisitor base;
	SymbolTable* symtab;
	Arena* arena;
} DeclPass;
void decl_init(DeclPass* d, Arena* arena);
void decl_visit_program(ASTVisitor* visitor, ASTNode* program);
void decl_visit_var(ASTVisitor* v, ASTNode* node);
void decl_visit_func(ASTVisitor* v, ASTNode* node);





#endif
