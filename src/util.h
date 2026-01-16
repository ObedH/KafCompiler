#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "defs.h"

void ptabs(usize num_tabs);
char* str_split(const char* src, usize n);
void safe_print(const char* str, usize n);

#endif /* UTILS_H */
