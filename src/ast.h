#ifndef AST_H
#define AST_H

#include <stddef.h>
#include "token.h"
#include "defs.h"

typedef enum {
	// Top level
	NODE_PROGRAM,
	NODE_FUNCTION,

	// Declarations
	NODE_VAR_DECL,
	NODE_PARAM,

	// Statmenets
	NODE_BLOCK,
	NODE_RETURN_STMT,
	NODE_IF_STMT,
	NODE_WHILE_STMT,
	NODE_FOR_STMT,
	NODE_EXPR_STMT,

	// Expressions
	NODE_ASSIGN_EXPR,
	NODE_BINARY_EXPR,
	NODE_UNARY_EXPR,
	NODE_CALL_EXPR,

	// Primary expressions
	NODE_LITERAL_INT,
	NODE_IDENTIFIER,

	// Types
	NODE_TYPE_NAME
} NodeType;
typedef enum {
	BINOP_PLUS,
	BINOP_MINUS,
	BINOP_TIMES,
	BINOP_DIV,
	BINOP_EQ,
	BINOP_NEQ,
	BINOP_GT,
	BINOP_LT,
	BINOP_GE,
	BINOP_LE,
	BINOP_LOG_AND,
	BINOP_LOG_OR,
	BINOP_LOG_XOR
} BinOp;
const char* binop_str(BinOp binop);
typedef enum {
	UNOP_NEG,
	UNOP_LOG_NOT,
	UNOP_DEREF,
	UNOP_REF
} UnOp;
const char* unop_str(UnOp unop);

typedef enum {
	TYPE_U8,
	TYPE_I8,
	TYPE_U16,
	TYPE_I16,
	TYPE_U32,
	TYPE_I32,
	TYPE_U64,
	TYPE_I64,

	TYPE_PTR
} TypeKind;

typedef struct s_ast_node {
	NodeType node_type;
	usize line;
	usize col;
	
	union {
		struct {
			struct s_ast_node** decls;
			usize decl_count;
			usize capacity;
		} prgm;
		struct {
			String name;
			struct s_ast_node* return_type;
			struct s_ast_node** params;
			usize param_count; // max = 16;
			struct s_ast_node* body;
		} func;

		struct {
			String name;
			struct s_ast_node* type;
			struct s_ast_node* initializer;
		} var_declr;
		struct {
			String name;
			struct s_ast_node* type;
		} param;

		struct {
			struct s_ast_node** stmts;
			usize stmt_count;
			usize capacity;
		} block;
		struct {
			struct s_ast_node* expr;
		} return_stmt;
		struct {
			struct s_ast_node* cond;
			struct s_ast_node* then_branch;
			struct s_ast_node* else_branch; // Nullable
		} if_stmt;
		struct {
			struct s_ast_node* cond;
			struct s_ast_node* body;
		} while_stmt;
		struct {
			struct s_ast_node* expr;
		} expr_stmt;

		struct {
			struct s_ast_node* target;
			struct s_ast_node* value;
		} assign_expr;
		struct {
			BinOp op;
			struct s_ast_node* left;
			struct s_ast_node* right;
		} binary_expr;
		struct {
			UnOp op;
			struct s_ast_node* operand;
		} unary_expr;
		struct {
			struct s_ast_node* callee;
			struct s_ast_node** args;
			usize arg_count;
		} call_expr;

		struct {
			usize value;
		} literal_int;
		struct {
			String name;
		} identifier;

		struct {
			String name;
			TypeKind kind;
			struct s_ast_node* base; // Only for TYPE_PTR
		} type;
	};
} ASTNode;

ASTNode* ast_node_create(void);
void ast_node_print(ASTNode* ast_node, usize level);
void ast_node_free(ASTNode* ast_node);

ASTNode* ast_program_node_create(void);
void ast_program_node_append(ASTNode* p, ASTNode* a);
void ast_program_node_print(ASTNode* p, usize level);
void ast_program_node_free(ASTNode* p);

ASTNode* ast_func_node_create(void);
void ast_func_node_add_param(ASTNode* fn, ASTNode* p);
void ast_func_node_init(ASTNode* fn, String name, ASTNode* body, ASTNode* ret_type);
void ast_func_node_print(ASTNode* fn, usize level);
void ast_func_node_free(ASTNode* fn);

ASTNode* ast_type_node_create(void);
void ast_type_node_init(ASTNode* t, String name);
void ast_type_node_print(ASTNode* t, usize level);
void ast_type_node_free(ASTNode* t);

ASTNode* ast_param_node_create(void);
void ast_param_node_init(ASTNode* t, String name, ASTNode* type);
void ast_param_node_print(ASTNode* t, usize level);
void ast_param_node_free(ASTNode* t);

ASTNode* ast_block_node_create(void);
void ast_block_node_add_stmt(ASTNode* b, ASTNode* s);
void ast_block_node_print(ASTNode* b, usize level);
void ast_block_node_free(ASTNode* b);

void ast_stmt_node_print(ASTNode* s, usize level);
void ast_stmt_node_free(ASTNode* s);

ASTNode* ast_return_node_create(void);
void ast_return_node_init(ASTNode* r, ASTNode* e);
void ast_return_node_print(ASTNode* r, usize level);
void ast_return_node_free(ASTNode* r);

void ast_expr_node_print(ASTNode* e, usize level);
void ast_expr_node_free(ASTNode* e);

ASTNode* ast_assign_expr_node_create(ASTNode* left, ASTNode* value);
void ast_assign_expr_node_print(ASTNode* a, usize level);
void ast_assign_expr_node_free(ASTNode* a);

ASTNode* ast_literal_int_node_create(usize value);
void ast_literal_int_node_print(ASTNode* i, usize level);
void ast_literal_int_node_free(ASTNode* i);

ASTNode* ast_identifier_node_create(String name);
void ast_identifier_node_print(ASTNode* i, usize level);
void ast_identifier_node_free(ASTNode* i);

ASTNode* ast_binary_expr_node_create(String lexeme, ASTNode* left, ASTNode* right);
void ast_binary_expr_node_print(ASTNode* b, usize level);
void ast_binary_expr_node_free(ASTNode* b);

ASTNode* ast_unary_expr_node_create(String lexeme, ASTNode* operand);
void ast_unary_expr_node_print(ASTNode* u, usize level);
void ast_unary_expr_node_free(ASTNode* u);

#endif /* AST_H */
