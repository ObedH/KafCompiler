#ifndef IR_GEN_H
#define IR_GEN_H

#include "../symtab.h"
#include "../arena.h"
#include "../ast.h"
#include "../ir.h"

/* -------------------- IR PASS API -------------------- */

typedef struct {
	bool verbose;
	bool had_error;
	IRNode* ir_head;
	IRNode* ir_tail;
	SymbolTable* symtab;
	Arena* arena;
} IRPass;
void ir_init(IRPass* i, Arena* arena, SymbolTable* global, bool verbose);
void ir_free(IRPass* i);

/* -------------------- TOP LEVEL VISITING -------------------- */

void ir_visit_program(IRPass* i, ASTNode* node);
void ir_visit_func(IRPass* i, ASTNode* node);
void ir_visit_var(IRPass* i, ASTNode* node);


/* -------------------- STATEMENT VISITING -------------------- */

void ir_visit_stmt(IRPass* i, ASTNode* node);
void ir_visit_block(IRPass* i, ASTNode* node);
void ir_visit_return_stmt(IRPass* i, ASTNode* node);
void ir_visit_for_stmt(IRPass* i, ASTNode* node);
void ir_visit_expr_stmt(IRPass* i, ASTNode* node);

/* -------------------- STATEMENT VISITING -------------------- */

IROperand ir_visit_expr(IRPass* i, ASTNode* node);
IROperand ir_visit_bin_expr(IRPass* i, ASTNode* node);
IROperand ir_visit_literal(IRPass* i, ASTNode* node);
IROperand ir_visit_identifier(IRPass* i, ASTNode* node);

#endif
