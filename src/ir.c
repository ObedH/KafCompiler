#include "ir.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

/* -------------------- GLOBAL ID COUNTERS -------------------- */

static u32 LABEL_ID_COUNTER = 0;
static u32 LOCAL_ID_COUNTER = 0;

u32 ir_make_label_id(void) {
	return LABEL_ID_COUNTER++;
}
u32 ir_make_local_id(void) {
	return LOCAL_ID_COUNTER++;
}

/* -------------------- ENUM TO STRING FUNCTIONS -------------------- */

const char* ir_node_type_str(IRNodeType n) {
	switch(n) {
		case IR_ARITH:
			return "ARITHMETIC";
		case IR_CMP:
			return "COMPARISON";
		case IR_UNARY:
			return "UNARY";
		case IR_COPY:
			return "COPY";
		case IR_GOTO:
			return "GOTO";
		case IR_IF:
			return "IF";
		case IR_LABEL:
			return "LABEL";
		case IR_PARAM:
			return "PARAM";
		case IR_CALL:
			return "CALL";
		case IR_RETURN:
			return "RETURN";
		default:
			return "UNKNOWN";
	}
}
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
		case IR_MOD:
			return "modulus";
		case IR_BIT_AND:
			return "bitwise and";
		case IR_BIT_OR:
			return "bitwise or";
		case IR_BIT_XOR:
			return "bitwise xor";
		case IR_LSHIFT:
			return "left shift";
		case IR_RSHIFT:
			return "right shift";
		default:
			return "UNKNOWN";
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
			return "UNKNOWN";
	}
}
static const char* unary_op_str(IRUnaryOp op) {
	switch(op) {
		case IR_NEG:
			return "negation";
		case IR_BIT_NOT:
			return "bitwise not";
		case IR_LOG_NOT:
			return "logical not";
		default:
			return "UNKNOWN";
	}
}
void ir_operand_print(IROperand operand) {
	if(operand.operand_type == IR_CONST) {
		printf("%lu", operand.val);
	}
	else if(operand.operand_type == IR_VREG) {
		printf("t%u", operand.vreg);
	}
	else {
		printf("error");
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
		case IR_CMP:
			ir_cmp_print(node, level);
			break;
		case IR_UNARY:
			ir_unary_print(node, level);
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
			printf("Unknown IR Node type %s %u!\n", ir_node_type_str(node->type), node->type);
			break;
	}
}
void ir_node_free(IRNode* node) {
	free(node);
}

/* -------------------- ARITHMETIC NODE FUNCTIONS -------------------- */

IRNode* ir_arith_create(IRArithOp op, u32 to, IROperand from, IROperand from2) {
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
	printf("Operand 1: ");
	ir_operand_print(node->arith.from);
	printf("\n");
	ptabs(level);
	printf("Operand 2: ");
	ir_operand_print(node->arith.from2);
	printf("\n");
	ptabs(level);
	printf("--------------------\n");
}

/* -------------------- COMPARE NODE FUNCTIONS -------------------- */

IRNode* ir_cmp_create(IRCmpOp op, u32 to, IROperand from, IROperand from2) {
	IRNode* tmp = ir_node_create();
	tmp->type = IR_CMP;
	tmp->cmp.op = op;
	tmp->cmp.to = to;
	tmp->cmp.from = from;
	tmp->cmp.from2 = from2;
	return tmp;
}
void ir_cmp_print(IRNode* node, usize level) {
	ptabs(level);
	printf("-----CMP-----\n");
	ptabs(level);
	printf("Operation: %s\n", cmp_op_str(node->cmp.op));
	ptabs(level);
	printf("Destination: t%u\n", node->cmp.to);
	ptabs(level);
	printf("Operand 1: ");
	ir_operand_print(node->cmp.from);
	printf("\n");
	ptabs(level);
	printf("Operand 2: ");
	ir_operand_print(node->cmp.from2);
	printf("\n");
	ptabs(level);
	printf("-------------\n");
}

/* -------------------- UNARY NODE FUNCTIONS -------------------- */

IRNode* ir_unary_create(IRUnaryOp op, u32 to, IROperand from) {
	IRNode* tmp = ir_node_create();
	tmp->type = IR_UNARY;
	tmp->unary.op = op;
	tmp->unary.to = to;
	tmp->unary.from = from;
	return tmp;
}
void ir_unary_print(IRNode* node, usize level) {
	ptabs(level);
	printf("-----UNARY-----\n");
	ptabs(level);
	printf("Operation: %s\n", unary_op_str(node->unary.op));
	ptabs(level);
	printf("Destination: t%u\n", node->unary.to);
	ptabs(level);
	printf("Operand: ");
	ir_operand_print(node->unary.from);
	printf("\n");
	ptabs(level);
	printf("---------------\n");
}

/* -------------------- COPY NODE FUNCTIONS -------------------- */

IRNode* ir_copy_create(u32 to, IROperand from) {
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
	printf("Source: ");
	ir_operand_print(node->copy.from);
	printf("\n");
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
	printf("--------------\n");
}

/* -------------------- IF NODE FUNCTIONS -------------------- */

IRNode* ir_if_create(IRCmpOp op, u32 tlbl, IROperand ta, IROperand tb) {
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
	printf("A: ");
	ir_operand_print(node->_if.ta);
	printf("\n");
	ptabs(level);
	printf("B: ");
	ir_operand_print(node->_if.tb);
	printf("\n");
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

IRNode* ir_param_create(IROperand tparam) {
	IRNode* tmp = ir_node_create();
	tmp->type = IR_PARAM;
	tmp->param.tparam = tparam;
	return tmp;
}
void ir_param_print(IRNode* node, usize level) {
	ptabs(level);
	printf("-----PARAM-----\n");
	ptabs(level);
	printf("Source: ");
	ir_operand_print(node->param.tparam);
	printf("\n");
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
	printf("--------------\n");
}

/* -------------------- RETURN NODE FUNCTIONS -------------------- */

IRNode* ir_return_create(IROperand t) {
	IRNode* tmp = ir_node_create();
	tmp->type = IR_RETURN;
	tmp->_return.t = t;
	return tmp;
}
void ir_return_print(IRNode* node, usize level) {
	ptabs(level);
	printf("-----RETURN-----\n");
	ptabs(level);
	printf("Value: ");
	ir_operand_print(node->_return.t);
	printf("\n");
	ptabs(level);
	printf("----------------\n");
}
