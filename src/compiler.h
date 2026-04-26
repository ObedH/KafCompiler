#ifndef COMPILER_H
#define COMPILER_H

#include "arraylist.h"
#include "source.h"
#include "token.h"
#include "parse.h"
#include "ast.h"
#include "arena.h"
#include "pass/decl_pass.h"
#include "pass/type_pass.h"

/* -------------------- COMPILER STRUCT -------------------- */

typedef struct {
	SourceFile* src;

	Lexer* lexer;
	TokenList* token_list;

	Parser* parser;
	ASTNode* root;

	Arena* symtab_arena;

	SymbolTable* global_symtab;
	DeclPass decl_pass;
	TypePass type_pass;

	bool had_token_error;
	bool had_parse_error;
	bool has_source;
	bool has_tokens;
	bool has_ast;

} BasicCompiler;

/* -------------------- COMPILER CREATION -------------------- */

BasicCompiler* comp_create(void);
void comp_free(BasicCompiler* comp);

/* -------------------- SOURCE FILE API -------------------- */

void comp_open_src(BasicCompiler* comp, const char* path);
void comp_close_src(BasicCompiler* comp);

/* -------------------- LEXER API -------------------- */

void comp_tokenize(BasicCompiler* comp);
void comp_print_tokens(BasicCompiler* comp);
void comp_free_tokens(BasicCompiler* comp);

/* -------------------- PARSER API -------------------- */

void comp_parse(BasicCompiler* comp);
void comp_print_ast(BasicCompiler* comp);
void comp_free_ast(BasicCompiler* comp);

/* -------------------- PASS API -------------------- */

void comp_setup_symtabs(BasicCompiler* comp);
void comp_decl_pass(BasicCompiler* comp, bool verbose);
void comp_type_pass(BasicCompiler* comp, bool verbose);
void comp_free_symtabs(BasicCompiler* comp);

#endif
