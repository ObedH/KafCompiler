#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "parse.h"

#define MAX_BUFFER_LENGTH 256

int main(int argc, char** argv) {
	if(argc > 1) {
		FILE* in_fptr;
		const char* filename = argv[1];
		
		in_fptr = fopen(filename, "r");
		if(in_fptr == NULL){
			perror("Error opening file");
			return EXIT_FAILURE;
		}

		fseek(in_fptr, 0, SEEK_END);
		long file_size = ftell(in_fptr);
		fseek(in_fptr, 0, SEEK_SET);

		char* file_content = (char*)malloc(file_size + 1);
		if(file_content == NULL) {
			perror("Error allocating memory");
			fclose(in_fptr);
			return EXIT_FAILURE;
		}

		fread(file_content, 1, file_size, in_fptr);
		file_content[file_size] = '\0';

		fclose(in_fptr);
		
		TokenList* token_list = tokenize(file_content);
		//token_list_print(token_list, 0);

		ASTNode* syntax_tree = parse(token_list);
		ast_node_print(syntax_tree, 0);

		token_list_free(token_list);
		ast_program_node_free(syntax_tree);

		free(file_content);

	}


	return EXIT_SUCCESS;
}
