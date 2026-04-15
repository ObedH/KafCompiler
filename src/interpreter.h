#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "visitor.h"
#include "ast.h"

typedef struct {
	ASTNode* root;
	ExprVisitor* expr_visitor;
} Interpreter;






#endif
