#ifndef IR_H
#define IR_H

typedef enum {
	IR_ASSIGN;
} IRNodeType;

typedef enum {
	IR_TReg,
	IR_V
} IRAddressType;
typedef struct {
	IRAddressType type;
	union {
		struct {
			u16 treg;
		} Treg;
		struct {
			u64 value;
		} Value;
	};
} IRAddress;

typedef struct {
	IRNodeType type;

} IRNode;





#endif
