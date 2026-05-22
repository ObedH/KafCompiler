#ifndef DISAS_H
#define DISAS_H

#include <stdio.h>
#include "ir.h"

void disas_ir(FILE* output, IRNode* head, const char* opts);

#endif
