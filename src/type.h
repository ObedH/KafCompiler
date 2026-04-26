#ifndef TYPE_H
#define TYPE_H

#include "defs.h"
#include "string.h"

typedef enum {
	T_int_literal,
	T_float_literal,

	T_u8, T_i8, T_u16, T_i16, T_u32, T_i32, T_f32,
	T_u64, T_i64, T_f64, T_usize, T_isize, T_bool,
	T_string, T_void, T_char,
	T_array,
	T_function,
	T_class,
	T_error
} TypeKind;

typedef struct Type Type;

struct Type {
	TypeKind kind;

	union {
		struct {
			Type* element;
			usize size;
		} array;
		struct {
			Type* return_type;
			Type** param_types;
			usize param_count;
		} function;
		struct {
			String name;
		} class;
	};
};

TypeKind typekind_from_str(const char* str);

Type* type_make_primitive(TypeKind kind);
Type* type_make_array(Type* element, usize size);
Type* type_make_function(Type* return_type);
void type_function_add_param(Type* fuction, Type* param);
Type* type_make_class(String name);

Type* type_clone(Type* type);

bool type_is_numeric(Type* t);
bool type_is_integer(Type* t);
bool type_is_float(Type* t);
bool type_is_array(Type* t);
bool type_is_function(Type* t);
bool type_is_class(Type* t);
bool type_is_string(Type* t);
bool type_is_void(Type* t);
int type_rank(TypeKind k);

bool type_equals(Type* a, Type* b);
bool type_is_assignable(Type* a, Type* b);

const char* type_to_string(Type* t);
usize type_size(Type* t);

Type* type_function_return(Type* t);
Type* type_function_param(Type* t, usize i);

Type* type_base(Type* t);
usize type_array_size(Type* t);

void type_print(Type* type, usize l);
void type_free(Type* t);

#endif
