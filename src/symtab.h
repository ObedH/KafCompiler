#ifndef SYMTAB_H
#define SYMTAB_H

#include "hashmap.h"
#include "defs.h"
#include "type.h"
#include "arena.h"
#include "string.h"

/* -------------------- SYMBOL DATA STRUCTURES -------------------- */

typedef enum {
	SYM_FUNC,
	SYM_VAR
} SymbolType;
typedef enum {
	SC_GLOBAL,
	SC_BLOCK,
	SC_FUNCTION,
	SC_LOOP
} ScopeType;

typedef struct {
	SymbolType type;

	Type* resolved_type;

	union {
		struct {
			struct s_ast_node* node;
			u32 vreg;
			int stack_offset;
			Type* type;
			bool has_initializer;
			bool is_parameter;
		} var;
		struct {
			struct s_ast_node* node;
			Type* return_type;
			Type** param_types;
			String* param_names;
			usize param_count;
			bool is_defined;
			u32 label;
		} func;
	};
} Symbol;
Symbol* symbol_create(void);
Symbol* symbol_clone(Symbol* s);
void symbol_func_init(Symbol* symbol);
void symbol_func_add_param(Symbol* symbol, Type* type, String name);
void symbol_print(Symbol* symbol, usize l);
void symbol_free(Symbol* symbol);

typedef struct SymbolTable SymbolTable;

struct SymbolTable {
	HashMap* symbols;
	ScopeType scope_type;
	SymbolTable* parent;
};

/* -------------------- SYMBOL TABLE API -------------------- */

SymbolTable* symtab_create(Arena* arena, ScopeType scope_type);
void symtab_free(SymbolTable* symtab);

Symbol* symtab_lookup(SymbolTable* table, const char* name);
Symbol* symtab_lookup_current(SymbolTable* table, const char* name);
void symtab_insert(SymbolTable* table, const char* name, Symbol* symbol);

SymbolTable* symtab_push(Arena* arena, SymbolTable* current, ScopeType scope_type);
SymbolTable* symtab_pop(SymbolTable* current);

#endif
