#ifndef COMPILER_H
#define COMPILER_H

#include "arraylist.h"
#include "source.h"
#include "token.h"
#include "parse.h"
#include "ast.h"
#include "visitor.h"
#include "arena.h"

typedef struct {
	SourceFile* src;

	TokenScanner* token_scanner;
	TokenList* token_list;

	Parser* parser;
	ASTNode* root;

	Arena* symtab_arena;

	DeclPass decl_pass;

	bool had_token_error;
	bool had_parse_error;
	bool has_source;
	bool has_tokens;
	bool has_ast;

} BasicCompiler;

BasicCompiler* comp_create(void);
void comp_free(BasicCompiler* comp);

void comp_open_src(BasicCompiler* comp, const char* path);
void comp_close_src(BasicCompiler* comp);

void comp_tokenize(BasicCompiler* comp);
void comp_print_tokens(BasicCompiler* comp);
void comp_free_tokens(BasicCompiler* comp);

void comp_parse(BasicCompiler* comp);
void comp_print_ast(BasicCompiler* comp);
void comp_free_ast(BasicCompiler* comp);

void comp_setup_symtabs(BasicCompiler* comp);
void comp_decl_pass(BasicCompiler* comp);
void comp_free_symtabs(BasicCompiler* comp);

#endif
