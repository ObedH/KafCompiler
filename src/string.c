#include "string.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

String string_create(char* str, usize len) {
	String s;
	s.data = malloc(len + 1);
	if(!s.data) {
		perror("Error allocating memory for String!");
	}
	memcpy(s.data, str, len);
	s.data[len] = '\0';
	s.length = len;
	s.capacity = len + 1;
	return s;
}

void string_free(String s) {
	free(s.data);
}
