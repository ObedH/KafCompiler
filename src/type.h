#ifndef TYPE_H
#define TYPE_H

typedef enum {
	T_u8, T_i8, T_u16, T_i16, T_u32, T_i32, T_f32,
	T_u64, T_i64, T_f64, T_usize, T_isize, T_bool,
	T_string
} TypeKind;
typedef struct {
	TypeKind kind;
} Type;




#endif
