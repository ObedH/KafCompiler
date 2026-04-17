#include "arena.h"
#include <stdlib.h>
#include <stdio.h>

Arena* arena_create(void) {
	Arena* tmp = malloc(sizeof(*tmp));
	if(!tmp) {
		perror("Failed to allocate memory for Arena!");
		return NULL;
	}

	tmp->objects = malloc(sizeof(void*) * 64);
	tmp->num_objects = 0;
	tmp->capacity = 64;
	return tmp;
}
void arena_add_object(Arena* arena, void* object) {
	if(arena->num_objects == arena->capacity) {
		arena->capacity *= 2;
		void** tmp = realloc(arena->objects, sizeof(*tmp) * arena->capacity);
		if(!tmp) {
			perror("Failed to reallocate Arena!");
			return;
		}
		arena->objects = tmp;
	}
	arena->objects[arena->num_objects++] = object;
}
void arena_free(Arena* arena, ItemFreer free_func) {
	for(usize i = 0; i < arena->num_objects; i ++) {
		free_func(arena->objects[i]);
	}
	free(arena->objects);
	free(arena);
}
