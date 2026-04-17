#ifndef STRING_H
#define STRING_H

#include "defs.h"

typedef struct {
	usize length;
	usize capacity;
	char* data;
} String;
String string_create(char* str, usize len);
String string_dup(String str);
/*
void string_append(String* s, char c);
void string_append_str(String* s, const char* str);
String string_substr(const String* s, usize start, usize len);
*/
bool string_eq(String a, String b);
void string_free(String string);



#endif
