#ifndef K_STACK_GEN_PASS_H
#define K_STACK_GEN_PASS_H

#include "../defs.h"
#include "../ir.h"

typedef struct {
	bool verbose;
	bool had_error;
	IRNode* ir_head;
	u8* data;
	u32 size;
} KStackGenPass;
void k_stack_gen_init(KStackGenPass* k, IRNode* ir_head, bool verbose);
void k_stack_gen_free(KStackGenPass* k);



#endif

