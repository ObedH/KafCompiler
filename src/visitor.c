#include "visitor.h"

#include <stdio.h>

void visit_decl(ASTVisitor* v, ASTNode* node) {
	switch(node->node_type) {
		case NODE_VAR_DECL:
			if(v->visitVarDecl)
				v->visitVarDecl(v, node);
			break;
		case NODE_FUNCTION:
			if(v->visitFuncDecl)
				v->visitFuncDecl(v, node);
			break;
		default:
			printf("Expected a declaration when walking program AST!\n");
			break;
	}
}

Type* visit_type(ASTVisitor* v, ASTNode* node) {
	if(v->visitType) {
		return v->visitType(v, node);
	}
	return NULL;
}
