#include "tests.h"
#include "token.h"
#include "file.h"
#include <stdio.h>

bool test_token_init(void){
    printf("--------TEST--------\n");
    printf(" name=test_token_init\n");
    printf(" printing token...\n");
    struct s_token my_token = token_init(500, NULL, 0, 0);
	token_print(&my_token);
    bool success = true;
    printf(" success=%s\n", success ? "true": "false");
    printf("--------------------\n");
    return success;
}

bool test_file_read(const char *path){
    printf("--------TEST--------\n");
    printf(" name=test_file_read\n");
	char *file_contents = file_read(path);
	file_print(file_contents);
    file_free(file_contents);
    bool success = true;
    printf(" success=%s\n", success ? "true": "false");
    printf("--------------------\n");
    return success;
}