#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include <string.h>
#include "util.h"

/* -------------------- SYMBOL CREATION ------------------- */

Symbol* symbol_create(void) {
	Symbol* tmp = malloc(sizeof(*tmp));
	if(!tmp) {
		perror("Failed to allocate memory for symbol!");
		return NULL;
	}
	memset(tmp, 0, sizeof(*tmp));
	return tmp;
}
Symbol* symbol_clone(Symbol* s) {
	Symbol* tmp = symbol_create();
	memcpy(tmp, s, sizeof(*tmp));
	return tmp;
}
void symbol_func_init(Symbol* symbol) {
	symbol->func.param_count = 0;
	symbol->func.param_types = malloc(sizeof(Type*) * 16);
	symbol->func.param_names = malloc(sizeof(String) * 16);
}
void symbol_func_add_param(Symbol* symbol, Type* type, String name) {
	if(symbol->func.param_count == 16) {
		printf("Out of space for function parameters!\n");
		return;
	}
	usize i = symbol->func.param_count++;
	symbol->func.param_types[i] = type;
	symbol->func.param_names[i] = string_dup(name);
}
void symbol_print(Symbol* symbol, usize l) {
	if(!symbol) { 
		ptabs(l);
		printf("-----NULL-----\n");
		ptabs(l);
		printf("--------------\n");
		return;
	}
	ptabs(l);
	printf("-----SYMBOL-----\n");

	if(symbol->type == SYM_FUNC) {
		ptabs(l);
		printf("Type: Function\n");
		ptabs(l);
		printf("Return Type:\n");
		type_print(symbol->func.return_type, l + 1);

		ptabs(l);
		printf("Parameters:\n");
		for(usize i = 0; i < symbol->func.param_count; i ++) {
			ptabs(l + 1);
			printf("-----PARAM-----\n");

			ptabs(l + 1);
			printf("Name: %s\n", symbol->func.param_names[i].data);

			ptabs(l + 1);
			printf("Type:\n");
			type_print(symbol->func.param_types[i], l + 2);

			ptabs(l + 1);
			printf("---------------\n");
		}
	}
	else if(symbol->type == SYM_VAR) {
		ptabs(l);
		printf("Type: Variable\n");
		ptabs(l);
		printf("Variable type:\n");
		type_print(symbol->var.type, l + 1);
		ptabs(l);
		printf("Stack offset: %d\n", symbol->var.stack_offset);
		ptabs(l);
		printf("Has initializer: %s\n", symbol->var.has_initializer ? "true" : "false");
		ptabs(l);
		printf("Is parameter: %s\n", symbol->var.is_parameter ? "true" : "false");
	}
	else {
		ptabs(l);
		printf("Unknown symbol type!\n");
	}

	ptabs(l);
	printf("----------------\n");
}
void symbol_free(Symbol* symbol) {
	if(symbol->type == SYM_FUNC) {
		for(usize i = 0; i < symbol->func.param_count; i ++) {
			string_free(symbol->func.param_names[i]);
			type_free(symbol->func.param_types[i]);
		}
		free(symbol->func.param_names);
		free(symbol->func.param_types);
		//string_free(symbol->func.label);
	}
	else if(symbol->type == SYM_VAR) {
		type_free(symbol->var.type);
	}
	free(symbol);
}

/* -------------------- SYMBOL TABLE API ------------------- */

SymbolTable* symtab_create(Arena* arena, ScopeType sc) {
	SymbolTable* tmp = malloc(sizeof(*tmp));
	if(!tmp) {
		printf("Failed to allocate memory for symbol table!\n");
		return NULL;
	}
	memset(tmp, 0, sizeof(*tmp));
	tmp->symbols = hashmap_create();
	tmp->scope_type = sc;
	arena_add_object(arena, tmp);
	return tmp;
}
void symtab_free(SymbolTable* symtab) {
	hashmap_free(symtab->symbols, (ItemFreer)symbol_free);
	free(symtab);
}

Symbol* symtab_lookup(SymbolTable* table, const char* name) {
	SymbolTable* current = table;
	while(current != NULL) {
		Symbol* sym = hashmap_get(current->symbols, name);
		if(sym != NULL) {
			return sym;
		}
		current = current->parent;
	}
	return NULL;
}
Symbol* symtab_lookup_current(SymbolTable* table, const char* name) {
	return hashmap_get(table->symbols, name);
}
void symtab_insert(SymbolTable* table, const char* name, Symbol* symbol) {
	hashmap_put(table->symbols, name, symbol);
}

SymbolTable* symtab_push(Arena* arena, SymbolTable* current, ScopeType sc) {
	SymbolTable* new_symtab = symtab_create(arena, sc);
	new_symtab->parent = current;

	return new_symtab;
}
SymbolTable* symtab_pop(SymbolTable* current) {
	if(!current) {
		printf("Null current symtab!\n");
		return NULL;
	}
	return current->parent;
}
