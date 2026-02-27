#ifndef ASM_H
#define ASM_H

typedef enum {
	MOV,
	LEA,
	JMP,
	ADD,
	SUB
} ASMOp;

typedef enum {
	ASMDEST_REG1,
	ASMDEST_DIRMEM
} ASMDestType;
typedef union {
	u8 reg;
	u64 dirmem;
} ASMDestData;
typedef struct {
	ASMDestType type;
	ASMDestData data;
} ASMDest;
typedef enum {
	ASMSRC_REG1,
	ASMSRC_DIRMEM
} ASMSrcType;
typedef union {
	u8 reg;
	u64 dirmem;
} ASMSrcData;
typedef struct {
	ASMSrcType type;
	ASMSrcData data;
} ASMSrc;

typedef 

typedef struct s_asm_instr {
	ASMOp op,
	ASMDest dest,
	ASMSrc src1,
	ASMSrc src2,
	struct s_asm_instr* next;
} ASMInstr;

typedef struct {
	


} ASMProgram;

#endif
