#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "parse.h"
#include "compiler.h"

#define MAX_BUFFER_LENGTH 256

int main(int argc, char** argv) {
	if(argc > 1) {
		const char* filename = argv[1];
		BasicCompiler* comp = comp_create();

		comp_open_src(comp, filename);

		comp_tokenize(comp);

		//comp_print_tokens(comp);

		comp_parse(comp);

		//comp_print_ast(comp);

		comp_setup_symtabs(comp);
		comp_decl_pass(comp, false);
		comp_type_pass(comp, false);

		comp_free_symtabs(comp);
		
		comp_free_ast(comp);

		comp_free_tokens(comp);

		comp_close_src(comp);

		comp_free(comp);

		return EXIT_SUCCESS;
	}

	printf("Usage:\n");
	printf("\tcompiler [file_name]\n");
	return 1;
}
