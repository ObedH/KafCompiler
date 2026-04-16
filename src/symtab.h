#ifndef SYMTAB_H
#define SYMTAB_H

#include "hashmap.h"
#include "defs.h"
#include "type.h"
#include "string.h"

typedef enum {
	SYM_FUNC,
	SYM_VAR
} SymbolType;
typedef enum {
	SC_GLOBAL,
	SC_BLOCK
} ScopeType;

typedef struct {
	SymbolType type;
	union {
		struct {
			struct s_ast_node* node;
			int stack_offset;
			Type type;
			bool has_initializer;
			bool is_parameter;
		} var;
		struct {
			struct s_ast_node* node;
			Type return_type;
			Type* param_types;
			String* param_names;
			usize param_count;
			usize param_capacity;
			bool is_defined;
			String label;
		} func;
	};
} Symbol;
Symbol* symbol_create(void);
void symbol_func_init(Symbol* symbol);
void symbol_func_add_param(Symbol* symbol, Type type, const char* name);
void symbol_free(Symbol* symbol);

typedef struct SymbolTable SymbolTable;

/* SPAGHETTI STACK */
struct SymbolTable {
	HashMap* symbols;
	ScopeType scope_type;
	SymbolTable* parent;
	bool is_loop;
};
SymbolTable* symtab_create(ScopeType, bool is_loop);
void symtab_free(SymbolTable* symtab);

Symbol* symtab_lookup(SymbolTable* table, const char* name);
Symbol* symtab_lookup_current(SymbolTable* table, const char* name);
void symtab_insert(SymbolTable* table, const char* name, Symbol* symbol);

SymbolTable* symtab_push(SymbolTable* current);
SymbolTable* symtab_pop(SymbolTable* current);



#endif
