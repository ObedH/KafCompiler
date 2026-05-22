#include "disas.h"

static void disas_operand(FILE* output, IROperand operand) {
	switch(operand.operand_type) {
		case IR_CONST:
			fprintf(output, "%lu", operand.val);
			break;
		case IR_VREG:
			fprintf(output, "t%u", operand.vreg);
			break;
		default:
			fprintf(output, "(error)");
			break;
	}
}

static void disas_arith_operator(FILE* output, IRArithOp op) {
	switch(op) {
		case IR_ADD: fprintf(output, "+"); break;
		case IR_MUL: fprintf(output, "*"); break;
		case IR_SUB: fprintf(output, "-"); break;
		case IR_DIV: fprintf(output, "/"); break;
		case IR_MOD: fprintf(output, "%%"); break;
		case IR_BIT_AND: fprintf(output, "&"); break;
		case IR_BIT_OR: fprintf(output, "|"); break;
		case IR_BIT_XOR: fprintf(output, "^"); break;
		case IR_LSHIFT: fprintf(output, "<<"); break;
		case IR_RSHIFT: fprintf(output, ">>"); break;
		default: fprintf(output, "(error)"); break;
	}
}

static void disas_cmp_operator(FILE* output, IRCmpOp op) {
	switch(op) {
		case IR_EQUAL: fprintf(output, "=="); break;
		case IR_NOT_EQUAL: fprintf(output, "!="); break;
		case IR_GREATER_THAN: fprintf(output, ">"); break;
		case IR_LESS_THAN: fprintf(output, "<"); break;
		case IR_GREATER_THAN_OR_EQUAL: fprintf(output, ">="); break;
		case IR_LESS_THAN_OR_EQUAL: fprintf(output, "<="); break;
		default: fprintf(output, "(error)"); break;
	}
}

static void disas_arith(FILE* output, IRNode* node, const char* opts) {
	(void)opts;
	fprintf(output, "\tt%u := ", node->arith.to);
	disas_operand(output, node->arith.from);
	fprintf(output, " ");
	disas_arith_operator(output, node->arith.op);
	fprintf(output, " ");
	disas_operand(output, node->arith.from2);
	fprintf(output, "\n");
}
static void disas_cmp(FILE* output, IRNode* node, const char* opts) {
	(void)opts;
	fprintf(output, "\tt%u := ", node->cmp.to);
	disas_operand(output, node->cmp.from);
	fprintf(output, " ");
	disas_cmp_operator(output, node->cmp.op);
	fprintf(output, " ");
	disas_operand(output, node->cmp.from2);
	fprintf(output, "\n");
}
static void disas_copy(FILE* output, IRNode* node, const char* opts) {
	(void)opts;
	fprintf(output, "\tt%u := ", node->copy.to);
	disas_operand(output, node->copy.from);
	fprintf(output, "\n");
}
static void disas_goto(FILE* output, IRNode* node, const char* opts) {
	(void)opts;
	fprintf(output, "\tgoto .l%u\n", node->_goto.tlbl);
}
static void disas_if(FILE* output, IRNode* node, const char* opts) {
	(void)opts;
	fprintf(output, "\tif ");
	disas_operand(output, node->_if.ta);
	fprintf(output, " ");
	disas_cmp_operator(output, node->_if.op);
	fprintf(output, " ");
	disas_operand(output, node->_if.tb);
	fprintf(output, ": goto .l%u\n", node->_if.tlbl);
}
static void disas_label(FILE* output, IRNode* node, const char* opts) {
	(void)opts;
	fprintf(output, ".l%u\n", node->label.tlbl);
}
static void disas_return(FILE* output, IRNode* node, const char* opts) {
	(void)opts;
	fprintf(output, "\treturn ");
	disas_operand(output, node->_return.t);
	fprintf(output, "\n");
}

void disas_ir(FILE* output, IRNode* head, const char* opts) {
	for(IRNode* cur = head; cur; cur = cur->next) {
		switch(cur->type) {
			case IR_ARITH:
				disas_arith(output, cur, opts);
				break;
			case IR_CMP:
				disas_cmp(output, cur, opts);
				break;
			case IR_COPY:
				disas_copy(output, cur, opts);
				break;
			case IR_GOTO:
				disas_goto(output, cur, opts);
				break;
			case IR_IF:
				disas_if(output, cur, opts);
				break;
			case IR_LABEL:
				disas_label(output, cur, opts);
				break;
			case IR_RETURN:
				disas_return(output, cur, opts);
				break;
			default:
				printf("Unknown IR node type %s\n", ir_node_type_str(cur->type));
				break;
		}
	}
}
