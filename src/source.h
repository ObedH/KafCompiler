#ifndef SOURCE_H
#define SOURCE_H

#include "defs.h"

typedef struct {
	const char* source_name;
	const char* file_contents;
	char* new_path;
	const u64 file_length;
} SourceFile;

SourceFile* src_open(const char* path);
void src_free(SourceFile* src);









#endif
