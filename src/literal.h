#ifndef LITERAL_H
#define LITERAL_H

#include "defs.h"

typedef enum {
	LIT_INT,
	LIT_FLOAT,
	LIT_STRING,
	LIT_BOOL,
	LIT_CHAR
} LiteralKind;
const char* lit_type_str(LiteralKind k);

typedef struct {
	LiteralKind kind;

	union {
		u64 int_value;
		f64 float_value;
		struct {
			char* data;
			usize len;
		} string_value;
		bool bool_value;
		char char_value;
	};
} Literal;

#endif
