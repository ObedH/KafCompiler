#ifndef AST_H
#define AST_H

#include <stddef.h>
#include "token.h"
#include "defs.h"

typedef enum {
	NODE_PROGRAM,
	NODE_FUNCTION,
	NODE_VAR_DECL,
	NODE_PARAM,
	NODE_COMPOUND_STMT,
	NODE_RETURN_STMT,
	NODE_IF_STMT,
	NODE_WHILE_STMT,
	NODE_FOR_STMT,
	NODE_EXPR_STMT,

	NODE_EXPR,
	NODE_LITERAL_INT,
	NODE_IDENTIFIER,

	NODE_TYPE_NAME
} NodeType;

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
		} compound_stmt;
		struct {
			struct s_ast_node** expr;
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
			usize value;
		} literal_int;
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

ASTNode* ast_type_node_create(void);
void ast_type_node_init(ASTNode* t, String name);
void ast_type_node_print(ASTNode* t, usize level);
void ast_type_node_free(ASTNode* t);

ASTNode* ast_param_node_create(void);
void ast_param_node_init(ASTNode* t, String name, ASTNode* type);
void ast_param_node_print(ASTNode* t, usize level);
void ast_param_node_free(ASTNode* t);

#endif /* AST_H */
