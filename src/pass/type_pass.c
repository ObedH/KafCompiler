#include "type_pass.h"
#include "../type_resolver.h"

#include <string.h>
#include <stdio.h>

void type_init(TypePass* t, Arena* arena, SymbolTable* global_symtab, bool verbose) {
	memset(t, 0, sizeof(*t));
	t->arena = arena;
	t->symtab = global_symtab;
	t->verbose = verbose;
}

static TypeKind max_rank(Type* a, Type* b) {
	if(a->kind == T_error || b->kind == T_error) {
		return (T_error);
	}

	if(type_is_float(a) || type_is_float(b)) {
		if(a->kind == T_f64 || b->kind == T_f64) {
			return (T_f64);
		}
		return (T_f32);
	}

	if(a->kind == T_int_literal && type_is_integer(b)) {
		return b->kind;
	}
	if(b->kind == T_int_literal && type_is_integer(a)) {
		return a->kind;
	}

	if(a->kind == T_int_literal && b->kind == T_int_literal) {
		return (T_i32);
	}

	int ra = type_rank(a->kind);
	int rb = type_rank(b->kind);
	return ra > rb ? a->kind : b->kind;
}

static Type* promote_integer_type(Type* a, Type* b) {
	TypeKind k = max_rank(a, b);
	return type_make_primitive(k);
}

/* ----------------------------------- BINARY TYPE CHECKING -------------------------------------- */

static Type* binary_arithmetic(Type* a, Type* b) {
	if(type_is_float(a) || type_is_float(b)) {
		return type_make_primitive(T_f64);
	}

	if(type_is_integer(a) && type_is_integer(b)) {
		return promote_integer_type(a, b);
	}

	return type_make_primitive(T_error);
}
static Type* binary_comparison(Type* a, Type* b) {
	if(!type_is_numeric(a) || !type_is_numeric(b)) {
		return type_make_primitive(T_error);
	}

	return type_make_primitive(T_bool);
}
static Type* binary_equality(Type* a, Type* b) {
	if(!type_is_assignable(a, b) && !type_is_assignable(b, a)) {
		return type_make_primitive(T_error);
	}
	return type_make_primitive(T_bool);
}
static Type* binary_logical(Type* a, Type* b) {
	if(a->kind != T_bool || b->kind != T_bool) {
		return type_make_primitive(T_error);
	}
	return type_make_primitive(T_bool);
}
static Type* binary_bitwise(Type* a, Type* b) {
	if(!type_is_integer(a) || !type_is_integer(b)) {
		return type_make_primitive(T_error);
	}
	return promote_integer_type(a, b);
}

/* ----------------------------------- VISITING LOGIC -------------------------------------- */

void type_visit_program(TypePass* t, ASTNode* program) {

	if(t->verbose) printf("-----TYPE PASS-----\n");

	for(usize i = 0; i < program->prgm.decl_count; i ++) {
		ASTNode* decl = program->prgm.decls[i];
		type_visit_decl(t, decl);
	}

	if(t->verbose) printf("-------------------\n");
}
void type_visit_decl(TypePass* t, ASTNode* decl) {
	switch(decl->node_type) {
		case NODE_VAR_DECL:
			type_visit_var(t, decl);
			break;
		case NODE_FUNCTION:
			type_visit_func(t, decl);
			break;
		default:
			printf("Expected a delcaration when walking program AST!\n");
			break;
	}
}
void type_visit_var(TypePass* t, ASTNode* node) {
	if(!t) return;
	
	const char* name = node->var_declr.name.data;
	Symbol* sym = symtab_lookup(t->symtab, name);
	if(!sym) {
		printf("Could not find variable symbol: '%s' (line: %u, col: %u)\n", name, node->line, node->col);
		return;
	}

	sym->var.has_initializer = (node->var_declr.initializer != NULL);
	Type* initializer_type = NULL;
	if(node->var_declr.initializer) {
		initializer_type = type_visit_expr(t, node->var_declr.initializer);
	}
	if(!type_is_assignable(sym->var.type, initializer_type)) {
		printf("-----ERROR-----\n");
		printf("Type mismatch of variable '%s' (line %u, col %u)\n", name, node->line, node->col);
		printf("Initializer:\n");
		type_print(initializer_type, 1);
		printf("Type declaration:\n");
		type_print(sym->var.type, 1);
		printf("---------------\n");
		return;
	}
	sym->var.is_parameter = false;
	if(t->verbose) symbol_print(sym, 1);
}
void type_visit_func(TypePass* t, ASTNode* node) {
	if(!t) return;

	const char* name = node->func.name.data;

	Symbol* sym = symtab_lookup(t->symtab, name);
	if(!sym) {
		printf("Could not find function symbol: '%s' (line: %u, col: %u)\n", name, node->line, node->col);
		return;
	}

	Type* prev_return = t->current_return_type;
	t->current_return_type = sym->func.return_type;

	t->symtab = symtab_push(t->arena, t->symtab, SC_FUNCTION);

	for(usize i = 0; i < sym->func.param_count; i ++) {
		Type* param_type = sym->func.param_types[i];
		String param_name = sym->func.param_names[i];
		
		Symbol* param_sym = symbol_create();
		param_sym->type = SYM_VAR;
		param_sym->var.node = node->func.params[i];
		param_sym->var.type = param_type;
		param_sym->var.has_initializer = false;
		param_sym->var.is_parameter = true;
		param_sym->resolved_type = param_type;

		symtab_insert(t->symtab, param_name.data, param_sym);
		if(t->verbose) symbol_print(param_sym, 1);
	}

	type_visit_block(t, node->func.body, false);

	t->symtab = symtab_pop(t->symtab);

	t->current_return_type = prev_return;

}
void type_visit_block(TypePass* t, ASTNode* node, bool is_loop) {
	t->symtab = symtab_push(t->arena, t->symtab, is_loop ? SC_LOOP : SC_BLOCK);

	for(usize i = 0; i < node->block.stmt_count; i ++) {
		type_visit_stmt(t, node->block.stmts[i]);
	}
	
	t->symtab = symtab_pop(t->symtab);
}
void type_visit_stmt(TypePass* t, ASTNode* node) {
	switch(node->node_type) {
		case NODE_EXPR_STMT:
			type_visit_expr_stmt(t, node);
			break;
		case NODE_RETURN_STMT:
			type_visit_return_stmt(t, node);
			break;
		default:
			printf("Unknown statement type %u!\n", node->node_type);
			break;
	}
}
void type_visit_expr_stmt(TypePass* t, ASTNode* node) {
	type_visit_expr(t, node->expr_stmt.expr);
}
void type_visit_return_stmt(TypePass* t, ASTNode* node) {
	Type* expected_return_type = t->current_return_type;
	Type* received_return_type = type_make_primitive(T_void);
	if(node->return_stmt.expr) {
		received_return_type = type_visit_expr(t, node->return_stmt.expr);
	}

	if(!type_is_assignable(expected_return_type, received_return_type)) {
		printf("-----ERROR-----\n");
		printf("Return type mismatch! (line %u, col %u)\n", node->line, node->col);
		printf("Expected return type:\n");
		type_print(expected_return_type, 1);
		printf("Received return type:\n");
		type_print(received_return_type, 1);
		printf("---------------\n");
	}
}
Type* type_visit_expr(TypePass* t, ASTNode* node) {
	switch(node->node_type) {
		case NODE_ASSIGN_EXPR:
			return type_visit_assign_expr(t, node);
		case NODE_BINARY_EXPR:
			return type_visit_binary_expr(t, node);
			/*
		case NODE_UNARY_EXPR:
			return type_visit_unary_expr(t, node);
			*/
		case NODE_CALL_EXPR:
			return type_visit_call_expr(t, node);
		case NODE_LITERAL:
			return type_visit_literal(t, node);
		case NODE_IDENTIFIER:
			return type_visit_identifier(t, node);
		default:
			printf("Unknown expression type %u!\n", node->node_type);
			return NULL;
	}
}
Type* type_visit_assign_expr(TypePass* t, ASTNode* node) {
	Type* left_type = type_visit_expr(t, node->assign_expr.target);
	Type* right_type = type_visit_expr(t, node->assign_expr.value);

	if(!type_is_assignable(left_type, right_type)) {
		printf("-----ERROR-----\n");
		printf("Types are not compatible for assignment!\n");
		printf("Target:\n");
		type_print(left_type, 1);
		printf("Value:\n");
		type_print(right_type, 1);
		printf("---------------\n");
		return NULL;
	}
	return right_type;
}
Type* type_visit_binary_expr(TypePass* t, ASTNode* node) {
	Type* left_type = type_visit_expr(t, node->binary_expr.left);
	Type* right_type = type_visit_expr(t, node->binary_expr.right);
	switch(node->binary_expr.op) {
		case BINOP_PLUS:
		case BINOP_MINUS:
		case BINOP_TIMES:
		case BINOP_DIV:
		case BINOP_MOD:
			return binary_arithmetic(left_type, right_type);
		case BINOP_EQ:
		case BINOP_NEQ:
			return binary_equality(left_type, right_type);
		case BINOP_GT:
		case BINOP_LT:
		case BINOP_GE:
		case BINOP_LE:
			return binary_comparison(left_type, right_type);
		case BINOP_LOG_AND:
		case BINOP_LOG_OR:
		case BINOP_LOG_XOR:
			return binary_logical(left_type, right_type);
		case BINOP_BIT_AND:
		case BINOP_BIT_OR:
		case BINOP_BIT_XOR:
		case BINOP_LSHIFT:
		case BINOP_RSHIFT:
			return binary_bitwise(left_type, right_type);
		default:
			printf("Unkown binary operator: %u!\n", node->binary_expr.op);
			return type_make_primitive(T_error);
	}
}
Type* type_visit_call_expr(TypePass* t, ASTNode* node) {
	ASTNode* callee = node->call_expr.callee;
	Type* callee_signature = type_visit_expr(t, callee);
	if(callee_signature->kind != T_function) {
		printf("-----ERROR-----\n");
		printf("Attempting to call a non-function (line %u, col %u)\n", callee->line, callee->col);
		printf("---------------\n");
		return type_make_primitive(T_error);
	}

	const char* function_name = NULL;

	if(callee->node_type == NODE_IDENTIFIER) {
		function_name = callee->identifier.name.data;
	}

	if(node->call_expr.arg_count > callee_signature->function.param_count) {
		printf("-----ERROR-----\n");
		printf("Too many arguments for function '%s' (line %u, col %u)\n", function_name != NULL ? function_name : "(unknown)", node->line, node->col);
		printf("Expected %u args, got %u args\n", callee_signature->function.param_count, node->call_expr.arg_count);
		printf("---------------\n");
		return type_make_primitive(T_error);
	}
	if(node->call_expr.arg_count < callee_signature->function.param_count) {
		printf("-----ERROR-----\n");
		printf("Too few arguments for function '%s' (line %u, col %u)\n", function_name != NULL ? function_name : "(unknown)", node->line, node->col);
		printf("Expected %u args, got %u args\n", callee_signature->function.param_count, node->call_expr.arg_count);
		printf("---------------\n");
	}

	bool had_error = false;

	for(usize i = 0; i < callee_signature->function.param_count; i ++) {
		Type* param_type = callee_signature->function.param_types[i];
		ASTNode* arg_node = node->call_expr.args[i];
		Type* arg_type = type_visit_expr(t, arg_node);

		if(!type_is_assignable(param_type, arg_type)) {
			printf("Argument type mismatch when calling function '%s' (line %u, col %u)\n", function_name != NULL ? function_name : "(unknown)", arg_node->line, arg_node->col);
			printf("Expected type:\n");
			type_print(param_type, 1);
			printf("Got type:\n");
			type_print(arg_type, 1);
			printf("---------------\n");
			had_error = true;
		}
	}
	if(had_error) {
		return type_make_primitive(T_error);
	}
	
	return callee_signature->function.return_type;

}
Type* type_visit_literal(TypePass* t, ASTNode* node) {
	(void)t;
	switch(node->literal.lit.kind) {
		case LIT_INT: return type_make_primitive(T_int_literal);
		case LIT_FLOAT: return type_make_primitive(T_float_literal);
		case LIT_STRING: return type_make_primitive(T_string);
		case LIT_BOOL: return type_make_primitive(T_bool);
		case LIT_CHAR: return type_make_primitive(T_char);
		default:
			printf("Unkown literal type!\n");
			return type_make_primitive(T_error);
	}
}
Type* type_visit_identifier(TypePass* t, ASTNode* node) {
	Symbol* sym = symtab_lookup(t->symtab, node->identifier.name.data);
	if(!sym || !sym->resolved_type) {
		printf("-----ERROR-----\n");
		printf("Cannot find identifier '%s' (line %u, col %u)\n", node->identifier.name.data, node->line, node->col);
		printf("---------------\n");
		return type_make_primitive(T_error);
	}
	node->inferred_type = type_clone(sym->resolved_type);
	node->resolved_symbol = sym;
	return sym->resolved_type;
}
