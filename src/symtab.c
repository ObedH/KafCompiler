#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include <string.h>

Symbol* symbol_create(void) {
	Symbol* tmp = malloc(sizeof(*tmp));
	if(!tmp) {
		perror("Failed to allocate memory for symbol!");
		return NULL;
	}
	return tmp;
}
void symbol_func_init(Symbol* symbol) {
	symbol->func.param_capacity = 16;
	symbol->func.param_count = 0;
	symbol->func.param_types = malloc(sizeof(Type) * 16);
	symbol->func.param_names = malloc(sizeof(String) * 16);
}
void symbol_func_add_param(Symbol* symbol, Type type, const char* name) {
	if(symbol->func.param_count == symbol->func.param_capacity) {
		perror("Out of space for function parameters!");
		return;
	}
	symbol->func.param_types[symbol->func.param_count] = type;
	symbol->func.param_names[symbol->func.param_count] = string_create((char*)name, strlen(name));
}
void symbol_free(Symbol* symbol) {
	for(usize i = 0; i < symbol->func.param_count; i ++) {
		string_free(symbol->func.param_names[i]);
	}
	free(symbol->func.param_names);
	free(symbol->func.param_types);
	string_free(symbol->func.label);
	free(symbol);
}

SymbolTable* symtab_create(ScopeType sc, bool is_loop) {
	SymbolTable* tmp = malloc(sizeof(*tmp));
	if(!tmp) {
		perror("Failed to allocate memory for symbol table!");
		return NULL;
	}
	tmp->symbols = hashmap_create();
	tmp->scope_type = sc;
	tmp->is_loop = is_loop;
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
