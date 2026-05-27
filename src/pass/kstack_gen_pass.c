#include "kstack_gen_pass.h"
#include <stdlib.h>

void k_stack_gen_init(KStackGenPass* k, IRNode* ir_head, bool verbose) {
	k->verbose = verbose;
	k->had_error = false;
	k->ir_head = ir_head;
}
void k_stack_gen_free(KStackGenPass* k) {
	free(k->data);
}


