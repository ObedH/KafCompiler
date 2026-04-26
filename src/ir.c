#include "ir.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

/* -------------------- ENUM TO STRING FUNCTIONS -------------------- */

static const char* arith_op_str(IRArithOp op) {
	switch(op) {
		case IR_ADD:
			return "add";
		case IR_MUL:
			return "multiply";
		case IR_SUB:
			return "subtract";
		case IR_DIV:
			return "divide";
		default:
			printf("Unknown IR arithmetic operation type! %u\n", op);
			return NULL;
	}
}
static const char* cmp_op_str(IRCmpOp op) {
	switch(op) {
		case IR_EQUAL:
			return "equals";
		case IR_NOT_EQUAL:
			return "not equals";
		case IR_GREATER_THAN:
			return "greater than";
		case IR_LESS_THAN:
			return "less than";
		case IR_GREATER_THAN_OR_EQUAL:
			return "greater than or equal to";
		case IR_LESS_THAN_OR_EQUAL:
			return "less than or equal to";
		default:
			printf("Unknown IR comparison type! %u\n", op);
			return NULL;
	}
}

/* -------------------- GENERIC NODE FUNCTIONS -------------------- */

IRNode* ir_node_create(void) {
	IRNode* tmp = malloc(sizeof(*tmp));
	if(!tmp) {
		perror("Failed to mallocate memory for IRNode");
		return NULL;
	}

	return tmp;
}
void ir_node_print(IRNode* node, usize level) {
	if(!node) {
		ptabs(level);
		printf("-----NULL-----\n");
		ptabs(level);
		printf("--------------\n");
		return;
	}
	switch(node->type) {
		case IR_ARITH:
			ir_arith_print(node, level);
			break;
		case IR_COPY:
			ir_copy_print(node, level);
			break;
		case IR_GOTO:
			ir_goto_print(node, level);
			break;
		case IR_IF:
			ir_if_print(node, level);
			break;
		case IR_LABEL:
			ir_label_print(node, level);
			break;
		case IR_PARAM:
			ir_param_print(node, level);
			break;
		case IR_CALL:
			ir_call_print(node, level);
			break;
		case IR_RETURN:
			ir_return_print(node, level);
			break;
		default:
			ptabs(level);
			printf("Unknown IR Node type %u!\n", node->type);
			break;
	}
}
void ir_node_free(IRNode* node) {
	free(node);
}

/* -------------------- ARITHMETIC NODE FUNCTIONS -------------------- */

IRNode* ir_arith_create(IRArithOp op, u32 to, u32 from, u32 from2) {
	IRNode* tmp = ir_node_create();
	tmp->type = IR_ARITH;
	tmp->arith.op = op;
	tmp->arith.to = to;
	tmp->arith.from = from;
	tmp->arith.from2 = from2;
	return tmp;
}
void ir_arith_print(IRNode* node, usize level) {
	ptabs(level);
	printf("-----ARITHMETIC-----\n");
	ptabs(level);
	printf("Operation: %s\n", arith_op_str(node->arith.op));
	ptabs(level);
	printf("Destination: t%u\n", node->arith.to);
	ptabs(level);
	printf("Operand 1: t%u\n", node->arith.from);
	ptabs(level);
	printf("Operand 2: t%u\n", node->arith.from2);
	ptabs(level);
	printf("--------------------\n");
}

/* -------------------- COPY NODE FUNCTIONS -------------------- */

IRNode* ir_copy_create(u32 to, u32 from) {
	IRNode* tmp = ir_node_create();
	tmp->type = IR_COPY;
	tmp->copy.to = to;
	tmp->copy.from = from;
	return tmp;
}
void ir_copy_print(IRNode* node, usize level) {
	ptabs(level);
	printf("-----COPY-----\n");
	ptabs(level);
	printf("Destination: t%u\n", node->copy.to);
	ptabs(level);
	printf("Source: t%u\n", node->copy.from);
	ptabs(level);
	printf("--------------\n");
}

/* -------------------- GOTO NODE FUNCTIONS -------------------- */

IRNode* ir_goto_create(u32 tlbl) {
	IRNode* tmp = ir_node_create();
	tmp->type = IR_GOTO;
	tmp->_goto.tlbl = tlbl;
	return tmp;
}
void ir_goto_print(IRNode* node, usize level) {
	ptabs(level);
	printf("-----GOTO-----\n");
	ptabs(level);
	printf("Destination: l%u\n", node->_goto.tlbl);
	ptabs(level);
	printf("-----GOTO-----\n");
}

/* -------------------- IF NODE FUNCTIONS -------------------- */

IRNode* ir_if_create(IRCmpOp op, u32 tlbl, u32 ta, u32 tb) {
	IRNode* tmp = ir_node_create();
	tmp->type = IR_IF;
	tmp->_if.op = op;
	tmp->_if.tlbl = tlbl;
	tmp->_if.ta = ta;
	tmp->_if.tb = tb;
	return tmp;
}
void ir_if_print(IRNode* node, usize level) {
	ptabs(level);
	printf("-----IF-----\n");
	ptabs(level);
	printf("Operation: %s\n", cmp_op_str(node->_if.op));
	ptabs(level);
	printf("Destination: l%u\n", node->_if.tlbl);
	ptabs(level);
	printf("A: t%u\n", node->_if.ta);
	ptabs(level);
	printf("B: t%u\n", node->_if.tb);
	ptabs(level);
	printf("------------\n");
}

/* -------------------- LABEL NODE FUNCTIONS -------------------- */

IRNode* ir_label_create(u32 tlbl) {
	IRNode* tmp = ir_node_create();
	tmp->type = IR_LABEL;
	tmp->label.tlbl = tlbl;
	return tmp;
}
void ir_label_print(IRNode* node, usize level) {
	ptabs(level);
	printf("-----LABEL-----\n");
	ptabs(level);
	printf("ID: l%u\n", node->label.tlbl);
	ptabs(level);
	printf("---------------\n");
}

/* -------------------- PARAM NODE FUNCTIONS -------------------- */

IRNode* ir_param_create(u32 tparam) {
	IRNode* tmp = ir_node_create();
	tmp->type = IR_PARAM;
	tmp->param.tparam = tparam;
	return tmp;
}
void ir_param_print(IRNode* node, usize level) {
	ptabs(level);
	printf("-----PARAM-----\n");
	ptabs(level);
	printf("Source: t%u\n", node->param.tparam);
	ptabs(level);
	printf("---------------\n");
}

/* -------------------- CALL NODE FUNCTIONS -------------------- */

IRNode* ir_call_create(u32 tres, u32 tlbl) {
	IRNode* tmp = ir_node_create();
	tmp->type = IR_CALL;
	tmp->call.tres = tres;
	tmp->call.tlbl = tlbl;
	return tmp;
}
void ir_call_print(IRNode* node, usize level) {
	ptabs(level);
	printf("-----CALL-----\n");
	ptabs(level);
	printf("Destination: t%u\n", node->call.tres);
	ptabs(level);
	printf("Function: l%u\n", node->call.tlbl);
	ptabs(level);
	printf("-----CALL-----\n");
}

/* -------------------- RETURN NODE FUNCTIONS -------------------- */

IRNode* ir_return_create(u32 t) {
	IRNode* tmp = ir_node_create();
	tmp->_return.t = t;
	return tmp;
}
void ir_return_print(IRNode* node, usize level) {
	ptabs(level);
	printf("-----RETURN-----\n");
	ptabs(level);
	printf("Value: t%u\n", node->_return.t);
	ptabs(level);
	printf("-----RETURN-----\n");
}
