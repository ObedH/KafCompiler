#ifndef VISITOR_H
#define VISITOR_H

#include "ast.h"
#include "defs.h"
#include "string.h"

typedef enum {
	T_u8, T_i8, T_u16, T_i16,
	T_u32, T_i32, T_f32,
	T_u64, T_i64, T_f64,
	T_usize, T_isize,
	T_string
} ValueType;

typedef struct {
	ValueType type;
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

typedef struct ExprVisitor ExprVisitor;

struct ExprVisitor {
	Value (*visitAssignExpr)(ExprVisitor*, ASTNode*);
	Value (*visitBinaryExpr)(ExprVisitor*, ASTNode*);
	Value (*visitUnaryExpr)(ExprVisitor*, ASTNode*);
	Value (*visitCallExpr)(ExprVisitor*, ASTNode*);
	Value (*visitLiteralIntExpr)(ExprVisitor*, ASTNode*);
};

typedef struct StmtVisitor StmtVisitor;

struct StmtVisitor {
	void (*visitExpression)(StmtVisitor*, ASTNode*);
	void (*visitIf)(StmtVisitor*,ASTNode*);
	void (*visitWhile)(StmtVisitor*,ASTNode*);
	void (*visitReturn)(StmtVisitor*,ASTNode*);
	void (*visitBlock)(StmtVisitor*,ASTNode*);
};

typedef struct DeclVisitor DeclVisitor;

struct DeclVisitor {
	void (*visitVarDecl)(DeclVisitor*,ASTNode*);
	void (*visitFuncDecl)(DeclVisitor*,ASTNode*);
	void (*visitClassDecl)(DeclVisitor*,ASTNode*);
};

typedef struct {
	ExprVisitor expr_visitor;
	StmtVisitor stmt_visitor;
	DeclVisitor decl_visitor;
} ASTVisitor;


#endif
