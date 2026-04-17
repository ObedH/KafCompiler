#ifndef ARENA_H
#define ARENA_H

#include "defs.h"
#include "freer.h"

typedef struct {
	void** objects;
	usize num_objects;
	usize capacity;
} Arena;

Arena* arena_create(void);
void arena_add_object(Arena* arena, void* object);
void arena_free(Arena* arena, ItemFreer free_func);


#endif
