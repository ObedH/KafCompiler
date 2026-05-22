#include "ir_pass.h"
#include <stdio.h>

/* -------------------- IR PASS API -------------------- */

void ir_init(IRPass* i, Arena* arena, SymbolTable* global, bool verbose) {
	i->verbose = verbose;
	i->had_error = false;
	i->is_global = true;
	i->ir_head = NULL;
	i->ir_tail = NULL;
	i->symtab = global;
	i->arena = arena;
}
void ir_free(IRPass* i) {
	IRNode* cur = i->ir_head;
	while(cur) {
		IRNode* next = cur->next;
		ir_node_free(cur);
		cur = next;
	}
}
static void ir_emit(IRPass* i, IRNode* node) {
	if(!i->ir_tail) {
		node->next = NULL;
		node->prev = NULL;
		i->ir_head = node;
		i->ir_tail = node;
	}
	else {
		i->ir_tail->next = node;
		node->prev = i->ir_tail;
		node->next = NULL;
		i->ir_tail = node;
	}
	/*
	if(i->verbose) {
		ir_node_print(node, 1);
	}
	*/
}
static void emit_arith(IRPass* i, IRArithOp op, u32 to, IROperand from, IROperand from2) {
	IRNode* arith = ir_arith_create(op, to, from, from2);
	ir_emit(i, arith);
}
static void emit_cmp(IRPass* i, IRCmpOp op, u32 to, IROperand from, IROperand from2) {
	IRNode* cmp = ir_cmp_create(op, to, from, from2);
	ir_emit(i, cmp);
}
static void emit_copy(IRPass* i, u32 dest, IROperand src) {
	IRNode* copy = ir_copy_create(dest, src);
	ir_emit(i, copy);
}
static void emit_goto(IRPass* i, u32 tlbl) {
	IRNode* _goto = ir_goto_create(tlbl);
	ir_emit(i, _goto);
}
static void emit_if(IRPass* i, IRCmpOp op, u32 tlbl, IROperand a, IROperand b) {
	IRNode* _if = ir_if_create(op, tlbl, a, b);
	ir_emit(i, _if);
}
static u32 emit_label(IRPass* i) {
	u32 tlbl = ir_make_label_id();
	IRNode* label = ir_label_create(tlbl);
	ir_emit(i, label);
	return tlbl;
}
static void emit_return(IRPass* i, IROperand vreg) {
	IRNode* _return = ir_return_create(vreg);
	ir_emit(i, _return);
}
static IRArithOp ast_binop_to_ir_arithop(BinOp binop) {
	switch(binop) {
		case BINOP_PLUS: return IR_ADD;
		case BINOP_MINUS: return IR_SUB;
		case BINOP_TIMES: return IR_MUL;
		case BINOP_DIV: return IR_DIV;
		case BINOP_MOD: return IR_MOD;
		case BINOP_LSHIFT: return IR_LSHIFT;
		case BINOP_RSHIFT: return IR_RSHIFT;
		case BINOP_BIT_AND: return IR_BIT_AND;
		case BINOP_BIT_OR: return IR_BIT_OR;
		case BINOP_BIT_XOR: return IR_BIT_XOR;
		default:
			return -1;
	}
}
static IRCmpOp ast_binop_to_ir_cmpop(BinOp binop) {
	switch(binop) {
		case BINOP_EQ: return IR_EQUAL;
		case BINOP_NEQ: return IR_NOT_EQUAL;
		case BINOP_GT: return IR_GREATER_THAN;
		case BINOP_LT: return IR_LESS_THAN;
		case BINOP_GE: return IR_GREATER_THAN_OR_EQUAL;
		case BINOP_LE: return IR_LESS_THAN_OR_EQUAL;
		default:
			return -1;
	}
}
/* -------------------- TOP LEVEL VISITING -------------------- */

void ir_visit_program(IRPass* i, ASTNode* program) {
	if(i->verbose) printf("-----IR PASS-----\n");

	for(usize c = 0; c < program->prgm.decl_count; c ++) {
		ASTNode* decl = program->prgm.decls[c];
		switch(decl->node_type) {
			case NODE_FUNCTION:
				ir_visit_func(i, decl);
				break;
			case NODE_VAR_DECL:
				ir_visit_var(i, decl);
				break;
			default:
				printf("Unkown declaration type %s!\n", node_type_str(decl->node_type));
				i->had_error = true;
				break;
		}
	}

	if(i->verbose) printf("-------------------\n");
}

void ir_visit_func(IRPass* i, ASTNode* func) {
	if(i->had_error) return;
	Symbol* sym = func->resolved_symbol;

	i->is_global = false;
	//i->symtab = symtab_push(i->arena, i->symtab, SC_FUNCTION);

	for(usize c = 0; c < sym->func.param_count; c ++) {
		ASTNode* param = func->func.params[c];
		Symbol* sym = param->resolved_symbol;

		sym->var.vreg = ir_make_local_id();

		//symtab_insert(i->symtab, param->param.name.data, sym);
	}

	
	sym->func.label = emit_label(i);
	ir_visit_block(i, func->func.body);

	i->is_global = true;
	//i->symtab = symtab_pop(i->symtab);
}
void ir_visit_var(IRPass* i, ASTNode* var) {
	if(i->had_error) return;
	Symbol* sym = var->resolved_symbol;
	sym->var.vreg = ir_make_local_id();
	if(!i->is_global) {
		ASTNode* initializer = var->var_declr.initializer;
		IROperand expr_ir = ir_visit_expr(i, initializer);
		emit_copy(i, sym->var.vreg, expr_ir);
	}
}

/* -------------------- STATEMENT VISITING -------------------- */

void ir_visit_stmt(IRPass* i, ASTNode* stmt) {
	if(i->had_error) return;
	switch(stmt->node_type) {
		case NODE_BLOCK:
			ir_visit_block(i, stmt);
			break;
		case NODE_RETURN_STMT:
			ir_visit_return_stmt(i, stmt);
			break;
		case NODE_IF_STMT:
			ir_visit_if_stmt(i, stmt);
			break;
			/*
		case NODE_WHILE_STMT:
			ir_visit_while_stmt(i, stmt);
			break;
			*/
		case NODE_FOR_STMT:
			ir_visit_for_stmt(i, stmt);
			break;
		case NODE_EXPR_STMT:
			ir_visit_expr_stmt(i, stmt);
			break;
		case NODE_VAR_DECL:
			ir_visit_var(i, stmt);
			break;
		default:
			printf("Unknown statement type %s!\n", node_type_str(stmt->node_type));
			i->had_error = true;
			break;
	}
}
void ir_visit_block(IRPass* i, ASTNode* block) {
	if(i->had_error) return;

	//i->symtab = symtab_push(i->symtab, i->arena, SC_BLOCK);

	for(usize c = 0; c < block->block.stmt_count; c ++) {
		ASTNode* stmt = block->block.stmts[c];
		ir_visit_stmt(i, stmt);
	}

	//i->symtab = symtab_pop(i->symtab);
}
void ir_visit_expr_stmt(IRPass* i, ASTNode* node) {
	if(i->had_error) return;
	ir_visit_expr(i, node->expr_stmt.expr);
}
void ir_visit_return_stmt(IRPass* i, ASTNode* node) {
	if(i->had_error) return;
	if(node->return_stmt.expr) {
		IROperand vreg = ir_visit_expr(i, node->return_stmt.expr);
		emit_return(i, vreg);
	}
}
void ir_visit_for_stmt(IRPass* i, ASTNode* node) {
	if(i->had_error) return;
	ASTNode* init = node->for_stmt.init;
	ASTNode* cond = node->for_stmt.cond;
	ASTNode* update = node->for_stmt.update;
	ASTNode* stmt = node->for_stmt.stmt;

	if(init->node_type == NODE_VAR_DECL) {
		ir_visit_var(i, init);
	}
	else {
		ir_visit_expr_stmt(i, init);
	}

	u32 loop_start = emit_label(i);

	IROperand cond_ir = ir_visit_expr(i, cond);

	emit_if(i, IR_NOT_EQUAL, 0, cond_ir, (IROperand){.operand_type=IR_CONST,.val=0});
	IRNode* if_node = i->ir_tail;

	ir_visit_stmt(i, stmt);

	ir_visit_expr(i, update);
	emit_goto(i, loop_start);

	u32 loop_end = emit_label(i);
	if_node->_if.tlbl = loop_end;

}
void ir_visit_if_stmt(IRPass* i, ASTNode* node) {
	if(i->had_error) return;
	ASTNode* cond = node->if_stmt.cond;
	ASTNode* then_branch = node->if_stmt.then_branch;
	ASTNode* else_branch = node->if_stmt.else_branch;

	IROperand cond_operand = ir_visit_expr(i, cond);
	if(else_branch) {
		emit_if(i, IR_EQUAL, 0/*else_branch*/, cond_operand, (IROperand){.operand_type=IR_CONST,.val=0});
		IRNode* if_jmp = i->ir_tail;

		ir_visit_stmt(i, then_branch);
		emit_goto(i, 0/*finish*/);
		IRNode* goto_fin = i->ir_tail;
		u32 else_branch_label = emit_label(i);
		if_jmp->_if.tlbl = else_branch_label;

		ir_visit_stmt(i, else_branch);
		u32 finish_label = emit_label(i);
		goto_fin->_goto.tlbl = finish_label;
	}
	else {
		emit_if(i, IR_EQUAL, 0/*finish*/, cond_operand, (IROperand){.operand_type=IR_CONST,.val=0});
		IRNode* if_jmp = i->ir_tail;
		
		ir_visit_stmt(i, then_branch);
		u32 finish_label = emit_label(i);
		if_jmp->_if.tlbl = finish_label;
	}
}

/* -------------------- STATEMENT VISITING -------------------- */

IROperand ir_visit_expr(IRPass* i, ASTNode* node) {
	if(i->had_error) return (IROperand){.operand_type=IR_ERROR,.val=0};
	switch(node->node_type) {
		case NODE_ASSIGN_EXPR:
			return ir_visit_assign_expr(i, node);
		case NODE_BINARY_EXPR:
			return ir_visit_bin_expr(i, node);
		case NODE_LITERAL:
			return ir_visit_literal(i, node);
		case NODE_IDENTIFIER:
			return ir_visit_identifier(i, node);
		default:
			printf("Unknown expression type %s!\n", node_type_str(node->node_type));
			i->had_error = true;
			return (IROperand){.operand_type=IR_ERROR,.val=0};
	}
}
IROperand ir_visit_assign_expr(IRPass* i, ASTNode* node) {
	if(i->had_error) return (IROperand){.operand_type=IR_ERROR,.val=0};
	IROperand target = ir_visit_expr(i, node->assign_expr.target);
	IROperand value = ir_visit_expr(i, node->assign_expr.value);
	emit_copy(i, target.vreg, value);
	return target;
}
IROperand ir_visit_bin_expr(IRPass* i, ASTNode* node) {
	if(i->had_error) return (IROperand){.operand_type=IR_ERROR,.val=0};
	IROperand ta = ir_visit_expr(i, node->binary_expr.left);
	IROperand tb = ir_visit_expr(i, node->binary_expr.right);

	u32 tres = ir_make_local_id();

	if(ast_binop_to_ir_arithop(node->binary_expr.op) >= 0) {
		emit_arith(i, ast_binop_to_ir_arithop(node->binary_expr.op), tres, ta, tb);
	}
	else {
		emit_cmp(i, ast_binop_to_ir_cmpop(node->binary_expr.op), tres, ta, tb);
	}
	ir_node_print(i->ir_tail, 0);
	return (IROperand){.operand_type=IR_VREG, .vreg=tres};
}
IROperand ir_visit_literal(IRPass* i, ASTNode* node) {
	if(i->had_error) return (IROperand){.operand_type=IR_ERROR,.val=0};
	return (IROperand){.operand_type=IR_CONST,.val=node->literal.lit.int_value};
}
IROperand ir_visit_identifier(IRPass* i, ASTNode* node) {
	if(i->had_error) return (IROperand){.operand_type=IR_ERROR,.val=0};
	Symbol* sym = node->resolved_symbol;
	if(!sym) {
		printf("Could not find symbol %s\n", node->identifier.name.data);
		return (IROperand){.operand_type=IR_ERROR,.val=0};
	}
	return (IROperand){.operand_type=IR_VREG, .vreg=sym->var.vreg};
}
