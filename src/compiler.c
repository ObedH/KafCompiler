#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* -------------------- COMPILER CREATION -------------------- */

BasicCompiler* comp_create(void) {
	BasicCompiler* tmp = malloc(sizeof(*tmp));
	if(!tmp) {
		perror("Failed to allocate memory for Basic Compiler!");
		return NULL;
	}

	memset(tmp, 0, sizeof(*tmp));

	return tmp;
}
void comp_free(BasicCompiler* comp) {
	free(comp);
}

/* -------------------- SOURCE FILE API -------------------- */

void comp_open_src(BasicCompiler* comp, const char* path) {
	comp->src = src_open(path);
	comp->has_source = true;
}
void comp_close_src(BasicCompiler* comp) {
	if(!comp->has_source) {
		perror("No source provided!");
		return;
	}
	src_free(comp->src);
	comp->has_source = false;
}

/* -------------------- LEXER API -------------------- */

void comp_tokenize(BasicCompiler* comp) {
	if(!comp->has_source) {
		perror("No source file provided!");
		return;
	}
	comp->lexer = lexer_create();
	comp->token_list = lexer_lex_src(comp->lexer, comp->src);
	lexer_free(comp->lexer);
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

/* -------------------- PARSER API -------------------- */

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

/* -------------------- PASS API -------------------- */

void comp_setup_symtabs(BasicCompiler* comp) {
	comp->symtab_arena = arena_create();
	comp->global_symtab = symtab_create(comp->symtab_arena, SC_GLOBAL);
}
void comp_decl_pass(BasicCompiler* comp, bool verbose) {
	if(!comp->has_ast) {
		printf("No AST provided!\n");
		return;
	}

	decl_init(&comp->decl_pass, comp->symtab_arena, comp->global_symtab, verbose);
	decl_visit_program(&comp->decl_pass, comp->root);
	comp->has_decl_pass = true;
}
void comp_type_pass(BasicCompiler* comp, bool verbose) {
	if(!comp->has_decl_pass) {
		printf("Decl pass not completed!\n");
		return;
	}
	type_init(&comp->type_pass, comp->symtab_arena, comp->global_symtab, verbose);
	type_visit_program(&comp->type_pass, comp->root);
	comp->has_type_pass = true;
}
void comp_ir_pass(BasicCompiler* comp, bool verbose) {
	if(!comp->has_type_pass) {
		printf("Type pass not completed!\n");
		return;
	}

	ir_init(&comp->ir_pass, comp->symtab_arena, comp->global_symtab, verbose);
	ir_visit_program(&comp->ir_pass, comp->root);
	comp->has_ir_pass = true;
}
void comp_ir_nodes_free(BasicCompiler* comp) {
	if(!comp->has_ir_pass) {
		printf("IR pass not completed!\n");
		return;
	}
	
	ir_free(&comp->ir_pass);
	comp->has_ir_pass = false;
}
void comp_free_symtabs(BasicCompiler* comp) {
	arena_free(comp->symtab_arena, (ItemFreer)symtab_free);
}
