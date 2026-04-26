#ifndef IR_H
#define IR_H

/*
 * =========
 * Registers
 *
 * r0  r1  r2  r3  r4  r5  r6  r7  r8  r9  r10  r11  r12  r13  r14  r15
 * rax rbx rcx rdx rbp rsi rdi rsp r8  r9  r10  r11  r12  r13  r14  r15
 *
 *
 *
 *
 *
 */

typedef enum {
	IR_MOV,
} IRNodeType;
typedef enum {
	IR_ADD,
	IR_MUL,
	IR_SUB,
	IR_DIV
} IROp;
typedef enum {
	IR_Reg,
	IR_V
} IRAddressType;
typedef struct {
	IRAddressType type;
	union {
		struct {
			u16 reg;
		} reg;
		struct {
			u64 value;
		} value;
	};
} IRAddress;

typedef struct {
	IRNodeType type;
	union {
		struct {
			IROp op;
			IRAddress to;
			IRAddress from;
			IRAddress from2;
		} Assign;
	};
} IRNode;

IRNode* ir_node_create(void);
void ir_node_free(void);



#endif
