#ifndef K_STACK_H
#define K_STACK_H

typedef enum {
	KSTACKOP_ARITH,
	KSTACKOP_CMP,
	KSTACKOP_UNARY,
	KSTACKOP_COPY,
	KSTACKOP_GOTO,
	KSTACKOP_IF,
	KSTACKOP_PARAM,
	KSTACKOP_CALL,
	KSTACKOP_RETURN
} KStackOp;

typedef struct {
	KStackOp op_type;
} KStackInstruction;


#endif

