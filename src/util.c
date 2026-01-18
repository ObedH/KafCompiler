#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include <string.h>

void ptabs(usize num_tabs) {
	for(usize i = 0; i < num_tabs; i ++) {
		printf("    ");
	}
	return;
}

char* str_split(const char* src, usize n) {
	usize len = strlen(src);
	
	if(n > len) {
		n = len;
	}

	char* out = malloc(n + 1);
	if(!out) return NULL;

	memcpy(out, src, n);
	out[n] = '\0';

	return out;
}

void safe_print(const char* str, usize n) {
	for(usize i = 0; i < n; i ++) {
		char c = str[i];
		switch(c) {
			case '\n':
				printf("\\n");
				break;
			case '\r':
				printf("\\r");
				break;
			case '\t':
				printf("\\t");
				break;
			default:
				putchar(c);
				break;
		}
	}
	return;
}
