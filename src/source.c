#include "source.h"
#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>

SourceFile* src_open(const char* path) {
	FILE* fptr = fopen(path, "r");
	if(!fptr) {
		perror("Could not open source file!");
		return NULL;
	}

	SourceFile* src = malloc(sizeof(*src));
	if(!src) {
		perror("Memory allocation failed for SourceFile");
		fclose(fptr);
		return NULL;
	}

	char* new_path = strdup(path);
	const char* src_name = basename(new_path);
	fseek(fptr, 0, SEEK_END);
	const u64 file_length = ftell(fptr);
	fseek(fptr, 0, SEEK_SET);
	char* contents = malloc(file_length + 1);
	fread(contents, 1, file_length, fptr);
	contents[file_length] = '\0';
	SourceFile tmp = {.source_name = src_name, .file_length = file_length, .file_contents = contents, .new_path = new_path};
	memcpy(src, &tmp, sizeof(*src));

	fclose(fptr);
	return src;
}

void src_free(SourceFile* src) {
	free((void*)src->file_contents);
	free(src->new_path);
	free(src);
}
