#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>

BasicCompiler* comp_create(void) {
	BasicCompiler* tmp = malloc(sizeof(*tmp));
	if(!tmp) {
		perror("Failed to allocate memory for Basic Compiler!");
		return NULL;
	}

	return tmp;
}
void comp_open_src(BasicCompiler* comp, const char* path) {
	comp->src = src_open(path);
	comp->has_source = true;
}
void comp_tokenize(BasicCompiler* comp) {
	if(!comp->has_source) {
		perror("No source file provided!");
		return;
	}
	comp->token_scanner = tokscan_create();
	comp->token_list = tokscan_scan_src(comp->token_scanner, comp->src);
	tokscan_free(comp->token_scanner);
	comp->has_tokens = true;
}
void comp_print_tokens(BasicCompiler* comp) {
	if(!comp->has_tokens) {
		perror("No tokens provided!");
		return;
	}
	token_list_print(comp->token_list, 0);
}
void comp_free_tokens(BasicCompiler* comp) {
	if(!comp->has_tokens) {
		perror("No tokens provided!");
		return;
	}
	token_list_free(comp->token_list);
	comp->has_tokens = false;
}
void comp_parse(BasicCompiler* comp) {
	if(!comp->has_tokens) {
		perror("No tokens provided!");
		return;
	}
	comp->parser = parser_create();
	comp->root = parser_parse(comp->parser, comp->token_list);
	parser_free(comp->parser);
	comp->has_ast = true;
}
void comp_print_ast(BasicCompiler* comp) {
	if(!comp->has_ast) {
		perror("No AST provided!");
		return;
	}
	ast_node_print(comp->root, 0);
}
void comp_free_ast(BasicCompiler* comp) {
	if(!comp->has_ast) {
		perror("No AST provided!");
		return;
	}
	ast_program_node_free(comp->root);
	comp->has_ast = false;
}
void comp_decl_pass(BasicCompiler* comp) {
	if(!comp->has_ast) {
		perror("No AST provided!");
		return;
	}

	
}
void comp_close_src(BasicCompiler* comp) {
	if(!comp->has_source) {
		perror("No source provided!");
		return;
	}
	src_free(comp->src);
	comp->has_source = false;
}
void comp_free(BasicCompiler* comp) {
	free(comp);
}
