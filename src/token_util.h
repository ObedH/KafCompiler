#ifndef TOKEN_UTIL_H
#define TOKEN_UTIL_H

#include "defs.h"

static const char* keywords[] = {
	"break", 
	"case", 
	"fn",
	"pub",
	"var",
	"u8", 
	"i8", 
	"u16", 
	"i16", 
	"u32", 
	"i32", 
	"f32", 
	"u64", 
	"i64", 
	"f64", 
	"str",
	"usize", 
	"isize", 
	"const", 
	"continue", 
	"default", 
	"do", 
	"else", 
	"enum", 
	"for", 
	"goto", 
	"label", 
	"if", 
	"inline", 
	"return", 
	"sizeof", 
	"alignof", 
	"typeof", 
	"struct", 
	"switch", 
	"type", 
	"as", 
	"union", 
	"void", 
	"while"
};
#define NUM_KEYWORDS (sizeof(keywords)/sizeof(keywords[0]))
static const char* punctuators[] = {
	"->",
	"(",
	")",
	"[",
	"]",
	"{",
	"}",
	",",
	";",
	":",
	".",
	"?"
};
#define NUM_PUNCTUATORS (sizeof(punctuators)/sizeof(punctuators[0]))

static const char* operators[] = {
	"+",
	"-",
	"*",
	"/",
	"%",
	"++",
	"--",
	"&",
	"|",
	"^",
	"~",
	"<<",
	">>",
	"&&",
	"||",
	"!",
	"<",
	"<=",
	">",
	">=",
	"==",
	"!=",
	"=",
	"+=",
	"-=",
	"*=",
	"/=",
	"%=",
	"<<=",
	">>=",
	"&=",
	"^=",
	"|="
};
#define NUM_OPERATORS (sizeof(operators)/sizeof(operators[0]))


bool is_keyword(const char* lexeme, usize length) {
	for(usize i = 0; i < NUM_KEYWORDS; i ++) {
		const char* keyword = keywords[i];
		usize keyword_len = strlen(keyword);
		if(keyword_len == length && strncmp(lexeme, keyword, length) == 0) {
			return true;
		}
	}
	return false;
}
bool is_punctuator(const char* src, usize* out_len) {
	for(usize i = 0; i < NUM_PUNCTUATORS; i ++) {
		const char* punctuator = punctuators[i];
		usize punctuator_len = strlen(punctuator);

		if(strncmp(src, punctuator, punctuator_len) == 0) {
			if(out_len) {
				*out_len = punctuator_len;
				return true;
			}
			else {
				perror("Null pointer for out length!");
				return false;
			}
		}
	}
	return false;
}
bool is_operator(const char* src, usize* out_len) {
	for(usize i = 0; i < NUM_OPERATORS; i ++) {
		const char* op = operators[i];
		usize op_len = strlen(op);

		if(strncmp(src, op, op_len) == 0) {
			if(out_len) {
				*out_len = op_len;
				return true;
			}
			else {
				perror("Null pointer for out length!");
				return false;
			}
		}
	}
	return false;
}
bool is_number_literal(const char* src, usize* out_len) {
	usize i = 0;

	if(isdigit(src[0])) {
		for(i = 0; isdigit(src[i]); i ++);
	}
	else {
		*out_len = 0;
		return false;
	}

	*out_len = i;

	return true;
}


#endif // TOKEN_UTIL_H
