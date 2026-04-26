#ifndef AST_H
#define AST_H

#include <stddef.h>
#include "token.h"
#include "type.h"
#include "literal.h"
#include "symtab.h"
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
	NODE_LITERAL,
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
	BINOP_LOG_XOR,
	BINOP_BIT_AND,
	BINOP_BIT_OR,
	BINOP_BIT_XOR,
	BINOP_LSHIFT,
	BINOP_RSHIFT,
	BINOP_MOD
} BinOp;
const char* binop_str(BinOp binop);
typedef enum {
	UNOP_NEG,
	UNOP_LOG_NOT,
	UNOP_BIT_NOT
} UnOp;
const char* unop_str(UnOp unop);

typedef struct s_ast_node {
	NodeType node_type;
	usize line;
	usize col;

	Symbol* resolved_symbol;
	Type* inferred_type;
	
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
			struct s_ast_node* init;
			struct s_ast_node* cond;
			struct s_ast_node* update;
			struct s_ast_node* stmt;
		} for_stmt;
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
			Literal lit;
		} literal;
		struct {
			String name;
		} identifier;

		struct {
			String name;
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

ASTNode* ast_var_node_create(void);
void ast_var_node_init(ASTNode* var, String name, ASTNode* type, ASTNode* initializer);
void ast_var_node_print(ASTNode* var, usize level);
void ast_var_node_free(ASTNode* var);

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

ASTNode* ast_for_node_create(void);
void ast_for_node_init(ASTNode* f, ASTNode* i, ASTNode* c, ASTNode* u, ASTNode* b);
void ast_for_node_print(ASTNode* f, usize level);
void ast_for_node_free(ASTNode* f);

ASTNode* ast_while_node_create(void);
void ast_while_node_init(ASTNode* w, ASTNode* c, ASTNode* b);
void ast_while_node_print(ASTNode* w, usize level);
void ast_while_node_free(ASTNode* w);

ASTNode* ast_if_node_create(void);
void ast_if_node_init(ASTNode* i, ASTNode* c, ASTNode* t, ASTNode* e);
void ast_if_node_print(ASTNode* i, usize level);
void ast_if_node_free(ASTNode* i);

void ast_expr_node_print(ASTNode* e, usize level);
void ast_expr_node_free(ASTNode* e);

ASTNode* ast_expr_stmt_node_create(ASTNode* expr);
void ast_expr_stmt_node_print(ASTNode* es, usize level);
void ast_expr_stmt_node_free(ASTNode* es);

ASTNode* ast_assign_expr_node_create(ASTNode* left, ASTNode* value);
void ast_assign_expr_node_print(ASTNode* a, usize level);
void ast_assign_expr_node_free(ASTNode* a);

ASTNode* ast_literal_node_create(Literal lit);
void ast_literal_node_print(ASTNode* i, usize level);
void ast_literal_node_free(ASTNode* i);

ASTNode* ast_identifier_node_create(String name);
void ast_identifier_node_print(ASTNode* i, usize level);
void ast_identifier_node_free(ASTNode* i);

ASTNode* ast_binary_expr_node_create(String lexeme, ASTNode* left, ASTNode* right);
void ast_binary_expr_node_print(ASTNode* b, usize level);
void ast_binary_expr_node_free(ASTNode* b);

ASTNode* ast_unary_expr_node_create(String lexeme, ASTNode* operand);
void ast_unary_expr_node_print(ASTNode* u, usize level);
void ast_unary_expr_node_free(ASTNode* u);

ASTNode* ast_call_expr_node_create(void);
void ast_call_expr_node_init(ASTNode* c, ASTNode* callee);
void ast_call_expr_node_add_arg(ASTNode* c, ASTNode* arg);
void ast_call_expr_node_print(ASTNode* c, usize level);
void ast_call_expr_node_free(ASTNode* c);

#endif /* AST_H */
