#ifndef VISITOR_H
#define VISITOR_H

#include "ast.h"
#include "defs.h"
#include "symtab.h"
#include "string.h"
#include "type.h"

typedef struct {
	Type type;
	union {
		u8 u8_v;
		i8 i8_v;
		u16 u16_v;
		i16 i16_v;
		u32 u32_v;
		i32 i32_v;
		f32 f32_v;
		u64 u64_v;
		i64 i64_v;
		f64 f64_v;
		usize usize_v;
		isize isize_v;
		String string_v;
	};
} Value;

typedef struct ASTVisitor ASTVisitor;

struct ASTVisitor {
	Type* (*visitType)(ASTVisitor*, ASTNode*);

	Value (*visitAssignExpr)(ASTVisitor*, ASTNode*);
	Value (*visitBinaryExpr)(ASTVisitor*, ASTNode*);
	Value (*visitUnaryExpr)(ASTVisitor*, ASTNode*);
	Value (*visitCallExpr)(ASTVisitor*, ASTNode*);
	Value (*visitLiteralIntExpr)(ASTVisitor*, ASTNode*);

	void (*visitExpression)(ASTVisitor*, ASTNode*);
	void (*visitIf)(ASTVisitor*,ASTNode*);
	void (*visitWhile)(ASTVisitor*,ASTNode*);
	void (*visitReturn)(ASTVisitor*,ASTNode*);
	void (*visitBlock)(ASTVisitor*,ASTNode*);

	void (*visitVarDecl)(ASTVisitor*,ASTNode*);
	void (*visitFuncDecl)(ASTVisitor*,ASTNode*);
};

//Dispatch each decl into visit_decl
void visit_prgm(ASTVisitor* v, ASTNode* node);
//For each decl type, make the visitor visit it.
void visit_decl(ASTVisitor* v, ASTNode* node);

Type* visit_type(ASTVisitor* v, ASTNode* node);

#endif
