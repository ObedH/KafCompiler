#include "literal.h"

const char* lit_type_str(LiteralKind k) {
	switch(k) {
		case LIT_INT:
			return "INT";
		case LIT_FLOAT:
			return "FLOAT";
		case LIT_STRING:
			return "STRING";
		case LIT_BOOL:
			return "BOOL";
		case LIT_CHAR:
			return "CHAR";
		default:
			return "UNKNOWN";
	}
}
