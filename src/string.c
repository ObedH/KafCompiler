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
String string_dup(String str) {
	return string_create(str.data, str.length);
}
bool string_eq(String a, String b) {
	if(a.length != b.length) return false;
	return !strcmp(a.data, b.data);
}
void string_free(String s) {
	free(s.data);
}
