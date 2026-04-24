#ifndef TYPE_PASS_H
#define TYPE_PASS_H

#include "../symtab.h"
#include "../arena.h"
#include "../ast.h"

typedef struct {
	SymbolTable* symtab;
	Arena* arena;

	bool verbose;
} TypePass;
void type_init(TypePass* t, bool verbose, Arena* arena);
void type_visit_program(TypePass* t, ASTNode* node);
void type_visit_decl(TypePass* t, ASTNode* node);
void type_visit_func(TypePass* t, ASTNode* node);
Type* type_visit_type(TypePass* t, ASTNode* node);
void type_visit_block(TypePass* t, ASTNode* node, bool is_loop);
Type* type_visit_expr(TypePass* t, ASTNode* node);
Type* type_visit_assign_expr(TypePass* t, ASTNode* node);
Type* type_visit_binary_expr(TypePass* t, ASTNode* node);
Type* type_visit_unary_expr(TypePass* t, ASTNode* node);
Type* type_visit_call_expr(TypePass* t, ASTNode* node);
Type* type_visit_literal_int(TypePass* t, ASTNode* node);
Type* type_visit_identifier(TypePass* t, ASTNode* node);




#endif
