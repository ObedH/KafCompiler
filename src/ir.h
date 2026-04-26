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
	IR_COPY,
	IR_GOTO,
	IR_IF,
	IR_LABEL,
	IR_PARAM,
	IR_CALL,
	IR_RETURN
} IRNodeType;
typedef enum {
	IR_ADD,
	IR_MUL,
	IR_SUB,
	IR_DIV
} IRArithOp;
typedef enum {
	IR_EQUAL,
	IR_NOT_EQUAL,
	IR_GREATER_THAN,
	IR_LESS_THAN,
	IR_GREATER_THAN_OR_EQUAL,
	IR_LESS_THAN_OR_EQUAL
} IRCmpOp;

typedef struct {
	IRNodeType type;
	union {
		struct {
			IRArithOp op;
			u32 to;
			u32 from;
			u32 from2;
		} arith;
		struct {
			u32 to;
			u32 from;
		} copy;
		struct {
			u32 tlbl;
		} _goto;
		struct {
			IRCmpOp op;
			u32 tlbl;
			u32 ta;
			u32 tb;
		} _if;
		struct {
			u32 tlbl;	
		} label;
		struct {
			u32 tparam;
		} param;
		struct {
			u32 tres;
			u32 tlbl;
		} call;
		struct {
			u32 t;
		} _return;
	};
} IRNode;

/* -------------------- GENERIC NODE FUNCTIONS -------------------- */

IRNode* ir_node_create(void);
void ir_node_print(IRNode* node, usize level);
void ir_node_free(IRNode* node);

/* -------------------- ARITHMETIC NODE FUNCTIONS -------------------- */

IRNode* ir_arith_create(IRArithOp op, u32 to, u32 from, u32 from2);
void ir_arith_print(IRNode* node, usize level);

/* -------------------- COPY NODE FUNCTIONS -------------------- */

IRNode* ir_copy_create(u32 to, u32 from);
void ir_copy_print(IRNode* node, usize level);

/* -------------------- GOTO NODE FUNCTIONS -------------------- */

IRNode* ir_goto_create(u32 tlbl);
void ir_goto_print(IRNode* node, usize level);

/* -------------------- IF NODE FUNCTIONS -------------------- */

IRNode* ir_if_create(IRCmpOp op, u32 tlbl, u32 ta, u32 tb);
void ir_if_print(IRNode* node, usize level);

/* -------------------- LABEL NODE FUNCTIONS -------------------- */

IRNode* ir_label_create(u32 tlbl);
void ir_label_print(IRNode* node, usize level);

/* -------------------- PARAM NODE FUNCTIONS -------------------- */

IRNode* ir_param_create(u32 tparam);
void ir_param_print(IRNode* node, usize level);

/* -------------------- CALL NODE FUNCTIONS -------------------- */

IRNode* ir_call_create(u32 tres, u32 tlbl);
void ir_call_print(IRNode* node, usize level);

/* -------------------- RETURN NODE FUNCTIONS -------------------- */

IRNode* ir_return_create(u32 t);
void ir_return_print(IRNode* node, usize level);

#endif
