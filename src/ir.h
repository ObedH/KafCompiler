#ifndef IR_H
#define IR_H

#include "defs.h"

/* -------------------- IR NODE STRUCTURES -------------------- */

/*
 * ===
 * 3AC
 * 
 * ARITH: (op, tres, ta, tb)
 * COPY: (tdst, tsrc)
 * GOTO: (tlbl)
 * IF: (op, tlbl, ta, tb)
 * LABEL: (tlbl)
 * PARAM: (tparam)
 * CALL: (tres, tlbl)
 * RETURN: (Option<t>)
 *
 * ===
 */
typedef enum {
	IR_ARITH,
	IR_CMP,
	IR_UNARY,
	IR_COPY,
	IR_GOTO,
	IR_IF,
	IR_LABEL,
	IR_PARAM,
	IR_CALL,
	IR_RETURN
} IRNodeType;
const char* ir_node_type_str(IRNodeType n);
typedef enum {
	IR_ADD,
	IR_MUL,
	IR_SUB,
	IR_DIV,
	IR_MOD,
	IR_BIT_AND,
	IR_BIT_OR,
	IR_BIT_XOR,
	IR_LSHIFT,
	IR_RSHIFT
} IRArithOp;
typedef enum {
	IR_EQUAL,
	IR_NOT_EQUAL,
	IR_GREATER_THAN,
	IR_LESS_THAN,
	IR_GREATER_THAN_OR_EQUAL,
	IR_LESS_THAN_OR_EQUAL
} IRCmpOp;
typedef enum {
	IR_NEG,
	IR_BIT_NOT,
	IR_LOG_NOT
} IRUnaryOp;
typedef enum {
	IR_CONST,
	IR_VREG,
	IR_ERROR
} IROperandType;
typedef struct {
	IROperandType operand_type;
	union {
		u64 val;
		u32 vreg;
	};
} IROperand;
void ir_operand_print(IROperand operand);

typedef struct IRNode IRNode;

struct IRNode {
	IRNodeType type;
	union {
		struct {
			IRArithOp op;
			u32 to;
			IROperand from;
			IROperand from2;
		} arith;
		struct {
			IRCmpOp op;
			u32 to;
			IROperand from;
			IROperand from2;
		} cmp;
		struct {
			IRUnaryOp op;
			u32 to;
			IROperand from;
		} unary;
		struct {
			u32 to;
			IROperand from;
		} copy;
		struct {
			u32 tlbl;
		} _goto;
		struct {
			IRCmpOp op;
			u32 tlbl;
			IROperand ta;
			IROperand tb;
		} _if;
		struct {
			u32 tlbl;
		} label;
		struct {
			IROperand tparam;
		} param;
		struct {
			u32 tres;
			u32 tlbl;
		} call;
		struct {
			IROperand t;
		} _return;
	};

	IRNode* next;
	IRNode* prev;
};

/* -------------------- ID GENERATION -------------------- */

u32 ir_make_label_id(void);
u32 ir_make_local_id(void);

/* -------------------- GENERIC NODE FUNCTIONS -------------------- */

IRNode* ir_node_create(void);
void ir_node_print(IRNode* node, usize level);
void ir_node_free(IRNode* node);

/* -------------------- ARITHMETIC NODE FUNCTIONS -------------------- */

IRNode* ir_arith_create(IRArithOp op, u32 to, IROperand from, IROperand from2);
void ir_arith_print(IRNode* node, usize level);

/* -------------------- COMPARE NODE FUNCTIONS -------------------- */

IRNode* ir_cmp_create(IRCmpOp op, u32 to, IROperand from, IROperand from2);
void ir_cmp_print(IRNode* node, usize level);

/* -------------------- UNARY NODE FUNCTIONS -------------------- */

IRNode* ir_unary_create(IRUnaryOp op, u32 to, IROperand from);
void ir_unary_print(IRNode* node, usize level);

/* -------------------- COPY NODE FUNCTIONS -------------------- */

IRNode* ir_copy_create(u32 to, IROperand from);
void ir_copy_print(IRNode* node, usize level);

/* -------------------- GOTO NODE FUNCTIONS -------------------- */

IRNode* ir_goto_create(u32 tlbl);
void ir_goto_print(IRNode* node, usize level);

/* -------------------- IF NODE FUNCTIONS -------------------- */

IRNode* ir_if_create(IRCmpOp op, u32 tlbl, IROperand ta, IROperand tb);
void ir_if_print(IRNode* node, usize level);

/* -------------------- LABEL NODE FUNCTIONS -------------------- */

IRNode* ir_label_create(u32 tlbl);
void ir_label_print(IRNode* node, usize level);

/* -------------------- PARAM NODE FUNCTIONS -------------------- */

IRNode* ir_param_create(IROperand tparam);
void ir_param_print(IRNode* node, usize level);

/* -------------------- CALL NODE FUNCTIONS -------------------- */

IRNode* ir_call_create(u32 tres, u32 tlbl);
void ir_call_print(IRNode* node, usize level);

/* -------------------- RETURN NODE FUNCTIONS -------------------- */

IRNode* ir_return_create(IROperand t);
void ir_return_print(IRNode* node, usize level);

#endif
