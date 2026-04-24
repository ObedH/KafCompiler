#include "type.h"
#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include <string.h>

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
static Type TYPE_VOID = {.kind=T_void};
static Type TYPE_BOOL = {.kind=T_bool};
static Type TYPE_ERROR = {.kind=T_error};

static Type* type_create(TypeKind kind) {
	Type* tmp = malloc(sizeof(*tmp));
	if(!tmp) {
		perror("Failed to allocate memory for Type");
		return NULL;
	}
	tmp->kind = kind;
	return tmp;
}

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
	if(!strcmp(str, "bool")) return T_bool;
	if(!strcmp(str, "void")) return T_void;
	printf("-----TYPE ERROR-----\n");
	printf("Unknown type: '%s'!\n", str);
	printf("--------------------\n");
	return T_error;
}

Type* type_make_primitive(TypeKind kind) {
	switch(kind) {
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
Type* type_function_param(Type* function, usize i) {
	if(i >= function->function.param_count) {
		printf("Invalid function parameter type index!\n");
		return NULL;
	}
	return function->function.param_types[i];
}
Type* type_make_class(String name) {
	Type* tmp = type_create(T_class);
	tmp->class.name = string_dup(name);
	return tmp;
}

bool type_is_numeric(Type* t) {
	switch(t->kind) {
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
bool type_is_array(Type* t) { return t->kind == T_array; }
bool type_is_function(Type* t) { return t->kind == T_function; }
bool type_is_class(Type* t) { return t->kind == T_class; }
bool type_is_string(Type* t) { return t->kind == T_string; }
bool type_is_void(Type* t) { return t->kind == T_void; }

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

bool type_is_assignable(Type* a, Type* b) {
	if(type_equals(a, b)) return true;
	if(!a || !b) return false;
	switch(a->kind) {
		case T_array: return false;
		case T_function: return false;
		case T_class: return true;
		case T_string: return true;
		case T_void: return false;
		default:
			if(type_is_numeric(a)) {
				return type_is_numeric(b);
			}
			if(a->kind == T_bool) {
				return b->kind == T_bool;
			}
			printf("Unknown type kind!\n");
			return false

	}
}

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
			ptabs(l);
			printf("Kind: Primitive\n");
			ptabs(l);
			printf("Base: ");
			switch(t->kind) {
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
