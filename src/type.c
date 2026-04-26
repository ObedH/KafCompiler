#include "type.h"
#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include <string.h>

/* -------------------- PRIMITIVE TYPES -------------------- */

static Type TYPE_INT_LIT = {.kind=T_int_literal};
static Type TYPE_FLOAT_LIT = {.kind=T_float_literal};
static Type TYPE_U8 = {.kind=T_u8};
static Type TYPE_I8 = {.kind=T_i8};
static Type TYPE_U16 = {.kind=T_u16};
static Type TYPE_I16 = {.kind=T_i16};
static Type TYPE_U32 = {.kind=T_u32};
static Type TYPE_I32 = {.kind=T_i32};
static Type TYPE_F32 = {.kind=T_f32};
static Type TYPE_U64 = {.kind=T_u64};
static Type TYPE_I64 = {.kind=T_i64};
static Type TYPE_F64 = {.kind=T_f64};
static Type TYPE_USIZE = {.kind=T_usize};
static Type TYPE_ISIZE = {.kind=T_isize};
static Type TYPE_STRING = {.kind=T_string};
static Type TYPE_CHAR = {.kind=T_char};
static Type TYPE_VOID = {.kind=T_void};
static Type TYPE_BOOL = {.kind=T_bool};
static Type TYPE_ERROR = {.kind=T_error};

/* -------------------- TYPE KIND HELPER FUNCTIONS -------------------- */

TypeKind typekind_from_str(const char* str) {
	if(!strcmp(str, "u8")) return T_u8;
	if(!strcmp(str, "i8")) return T_i8;
	if(!strcmp(str, "u16")) return T_u16;
	if(!strcmp(str, "i16")) return T_i16;
	if(!strcmp(str, "u32")) return T_u32;
	if(!strcmp(str, "i32")) return T_i32;
	if(!strcmp(str, "f32")) return T_f32;
	if(!strcmp(str, "u64")) return T_u64;
	if(!strcmp(str, "i64")) return T_i64;
	if(!strcmp(str, "f64")) return T_f64;
	if(!strcmp(str, "usize")) return T_usize;
	if(!strcmp(str, "isize")) return T_isize;
	if(!strcmp(str, "string")) return T_string;
	if(!strcmp(str, "char")) return T_char;
	if(!strcmp(str, "bool")) return T_bool;
	if(!strcmp(str, "void")) return T_void;
	printf("-----TYPE ERROR-----\n");
	printf("Unknown type: '%s'!\n", str);
	printf("--------------------\n");
	return T_error;
}
int type_rank(TypeKind k) {
	switch(k) {
		case T_u8: return 1;
		case T_i8: return 2;
		case T_u16: return 3;
		case T_i16: return 4;
		case T_u32: return 5;
		case T_i32: return 6;
		case T_u64: return 7;
		case T_i64: return 8;
		case T_f32: return 9;
		case T_f64: return 10;
		case T_int_literal: return 0;
		case T_float_literal: return 0;
		default:
			printf("Unknown type %u!\n", k);
			return -1;
	}
}

/* -------------------- TYPE CREATION -------------------- */

static Type* type_create(TypeKind kind) {
	Type* tmp = malloc(sizeof(*tmp));
	if(!tmp) {
		perror("Failed to allocate memory for Type");
		return NULL;
	}
	tmp->kind = kind;
	return tmp;
}
Type* type_make_primitive(TypeKind kind) {
	switch(kind) {
		case T_int_literal: return &TYPE_INT_LIT;
		case T_float_literal: return &TYPE_FLOAT_LIT;
		case T_u8: return &TYPE_U8;
		case T_i8: return &TYPE_I8;
		case T_u16: return &TYPE_U16;
		case T_i16: return &TYPE_I16;
		case T_u32: return &TYPE_U32;
		case T_i32: return &TYPE_I32;
		case T_f32: return &TYPE_F32;
		case T_u64: return &TYPE_U64;
		case T_i64: return &TYPE_I64;
		case T_f64: return &TYPE_F64;
		case T_usize: return &TYPE_USIZE;
		case T_isize: return &TYPE_ISIZE;
		case T_string: return &TYPE_STRING;
		case T_char: return &TYPE_CHAR;
		case T_bool: return &TYPE_BOOL;
		case T_void: return &TYPE_VOID;
		case T_error: return &TYPE_ERROR;
		default:
			printf("Cannot make a primitive type out of %u!\n", kind);
			return NULL;
	}
}
Type* type_make_array(Type* element, usize size) {
	Type* tmp = type_create(T_array);
	tmp->array.element = element;
	tmp->array.size = size;
	return tmp;
}
Type* type_make_function(Type* return_type) {
	Type* tmp = type_create(T_function);
	tmp->function.return_type = return_type;
	tmp->function.param_count = 0;
	tmp->function.param_types = malloc(sizeof(*tmp->function.param_types) * 16);
	if(!tmp->function.param_types) {
		perror("Failed to allocate memory for param types");
	}
	return tmp;
}
void type_function_add_param(Type* function, Type* param) {
	if(function->function.param_count == 16) {
		printf("Functions have a max of 16 args!\n");
		return;
	}
	function->function.param_types[function->function.param_count++] = param;
}
Type* type_make_class(String name) {
	Type* tmp = type_create(T_class);
	tmp->class.name = string_dup(name);
	return tmp;
}
Type* type_clone(Type* t) {
	if(!t) return NULL;
	switch(t->kind) {
		case T_array:
			return type_make_array(t->array.element, t->array.size);
		case T_function:
			{
			Type* func = type_make_function(t->function.return_type);
			for(usize i = 0; i < t->function.param_count; i ++) {
				type_function_add_param(func, t->function.param_types[i]);
			}
			return func;
			}
		default:
			return type_make_primitive(t->kind);
	}
}

/* -------------------- TYPE KIND CHECKING -------------------- */

bool type_is_unsigned(Type* t) {
	switch(t->kind) {
		case T_u8:
		case T_u16:
		case T_u32:
		case T_u64:
		case T_usize:
			return true;
		default:
			return false;
	}
}
bool type_is_numeric(Type* t) {
	switch(t->kind) {
		case T_int_literal:
		case T_float_literal:
		case T_u8:
		case T_i8:
		case T_u16:
		case T_i16:
		case T_u32:
		case T_i32:
		case T_f32:
		case T_u64:
		case T_i64:
		case T_f64:
		case T_usize:
		case T_isize:
			return true;
		default:
			return false;
	}
}
bool type_is_integer(Type* t) {
	switch(t->kind) {
		case T_int_literal:
		case T_u8:
		case T_i8:
		case T_u16:
		case T_i16:
		case T_u32:
		case T_i32:
		case T_u64:
		case T_i64:
		case T_usize:
		case T_isize:
			return true;
		default:
			return false;
	}
}
bool type_is_float(Type* t) {
	switch(t->kind) {
		case T_f32:
		case T_f64:
		case T_float_literal:
			return true;
		default:
			return false;
	}
}
bool type_is_array(Type* t) { return t->kind == T_array; }
bool type_is_function(Type* t) { return t->kind == T_function; }
bool type_is_class(Type* t) { return t->kind == T_class; }
bool type_is_string(Type* t) { return t->kind == T_string; }
bool type_is_void(Type* t) { return t->kind == T_void; }

/* -------------------- TYPE COMPATIBILITY CHECKING -------------------- */

bool type_equals(Type* a, Type* b) {
	if(a == b) return true;
	if(!a || !b) return false;
	if(a->kind == T_error || b->kind == T_error) return true;
	if(a->kind != b->kind) return false;
	switch(a->kind) {
		case T_array:
			return a->array.size == b->array.size && type_equals(a->array.element, b->array.element);
		case T_function:
			if(a->function.param_count != b->function.param_count) return false;
			for(usize i = 0; i < a->function.param_count; i ++)
				if(!type_equals(a->function.param_types[i], b->function.param_types[i]))
					return false;
			return type_equals(a->function.return_type, b->function.return_type);
		case T_class:
			return string_eq(a->class.name, b->class.name);
		default:
			return true;
	}
}
bool int_literal_fits(Type* t, i64 v) {
	switch(t->kind) {
		case T_u8: return v >= 0 && v <= 255;
		case T_i8: return v >= -128 && v <= 127;
		case T_u16: return v >= 0 && v <= 65535;
		case T_i16: return v >= -32768 && v <= 32767;
		case T_u32: return v >= 0 && (u64)v <= 4294967295ULL;
		case T_i32: return v >= -2147483648LL && v <= 2147483647LL;
		case T_u64: return v >= 0;
		case T_i64: return true;
		default: return false;
	}
}
bool type_is_assignable(Type* target, Type* value) {
	if(!target || !value) return false;
	if(type_equals(target, value)) return true;

	if(type_is_void(target)) return false;
	if(target->kind == T_function) return false;
	if(target->kind == T_array) return false;

	if(value->kind == T_int_literal) {
		return type_is_integer(target);
	}

	if(value->kind == T_float_literal) {
		return type_is_float(target);
	}

	if(type_is_integer(target) && type_is_integer(value)) {
		return type_rank(value->kind) <= type_rank(target->kind);
	}
	if(type_is_float(target) && type_is_float(value)) {
		return type_rank(value->kind) <= type_rank(target->kind);
	}

	if(type_is_float(target) && type_is_integer(value)) {
		return true;
	}

	if(target->kind == T_bool) {
		return value->kind == T_bool;
	}
	if(target->kind == T_string) {
		return value->kind == T_string;
	}
	if(target->kind == T_class && value->kind == T_class) {
		return string_eq(target->class.name, value->class.name);
	}
	return false;
}

/* -------------------- TYPE GETTER FUNCTIONS -------------------- */

Type* type_function_param(Type* function, usize i) {
	if(i >= function->function.param_count) {
		printf("Invalid function parameter type index!\n");
		return NULL;
	}
	return function->function.param_types[i];
}

/* -------------------- OBJECT API FUNCTIONS -------------------- */

void type_print(Type* t, usize l) {
	if(!t) {
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
	ptabs(l);
	printf("-----TYPE-----\n");
	switch(t->kind) {
		case T_array:
			ptabs(l);
			printf("Kind: Array\n");
			ptabs(l);
			printf("Elements: %u\n", t->array.size);
			ptabs(l);
			printf("Base:\n");
			type_print(t->array.element, l + 1);
			break;
		case T_function:
			ptabs(l);
			printf("Kind: Function\n");
			ptabs(l);
			printf("Return type:\n");
			type_print(t->function.return_type, l + 1);
			ptabs(l);
			printf("Params:\n");
			for(usize i = 0; i < t->function.param_count; i ++) {
				type_print(type_function_param(t, i), l + 1);
			}
			break;
		case T_class:
			ptabs(l);
			printf("Kind: Class\n");
			ptabs(l);
			printf("Name: %s\n", t->class.name.data);
			break;
		case T_int_literal:
		case T_float_literal:
		case T_u8:
		case T_i8:
		case T_u16:
		case T_i16:
		case T_u32:
		case T_i32:
		case T_f32:
		case T_u64:
		case T_i64:
		case T_f64:
		case T_usize:
		case T_isize:
		case T_bool:
		case T_string:
		case T_void:
		case T_char:
			ptabs(l);
			printf("Kind: Primitive\n");
			ptabs(l);
			printf("Base: ");
			switch(t->kind) {
				case T_int_literal: printf("int literal"); break;
				case T_float_literal: printf("float literal"); break;
				case T_u8: printf("u8"); break;
				case T_i8: printf("i8"); break;
				case T_u16: printf("u16"); break;
				case T_i16: printf("i16"); break;
				case T_u32: printf("u32"); break;
				case T_i32: printf("i32"); break;
				case T_f32: printf("f32"); break;
				case T_u64: printf("u64"); break;
				case T_i64: printf("i64"); break;
				case T_f64: printf("f64"); break;
				case T_usize: printf("usize"); break;
				case T_isize: printf("isize"); break;
				case T_void: printf("void"); break;
				case T_bool: printf("bool"); break;
				case T_string: printf("string"); break;
				case T_char: printf("char"); break;
				default: printf("Unkown type"); break;
			}
			putchar('\n');
			break;
		default:
			ptabs(l);
			printf("Kind: Error\n");
			break;
	}
	ptabs(l);
	printf("--------------\n");
}
void type_free(Type* t) {
	if(!t) return;
	switch(t->kind) {
		case T_class:
			string_free(t->class.name);
			break;
		case T_array:
			type_free(t->array.element);
			break;
		case T_function:
			for(usize i = 0; i < t->function.param_count; i ++) {
				type_free(t->function.param_types[i]);
			}
			free(t->function.param_types);
			type_free(t->function.return_type);
			break;
		default:
			return;
	}

	free(t);
}
