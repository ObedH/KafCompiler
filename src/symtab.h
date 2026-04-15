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
			int stack_offset;
			Type type;
			ScopeType scope;
			String name;
			ASTNode* initializer;
			bool is_parameter;
		} var;
		struct {
			String name;
			Type return_type;
			ASTNode** params;
			usize param_count;
			bool is_defined;
			String label;
		} func;
	};
} Symbol;

typedef struct SymbolTable SymbolTable;

/* SPAGHETTI STACK */
struct SymbolTable {
	HashMap* symbols;
	SymbolTable* parent;
};

#endif
