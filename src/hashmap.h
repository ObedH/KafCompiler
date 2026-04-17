#ifndef HASHMAP_H
#define HASHMAP_H

#include "defs.h"
#include "freer.h"

typedef struct {
    char* key;
    void* value;
} HashMapEntry;
typedef struct {
    HashMapEntry* entries;
    usize capacity;
    usize size;
} HashMap;

HashMap* hashmap_create(void);
void hashmap_put(HashMap* map, const char* key, void* value);
void* hashmap_get(HashMap* map, const char* key);
void hashmap_put_raw(HashMap* map, u64 key, void* value);
void* hashmap_get_raw(HashMap* map, u64 key);

void hashmap_print(HashMap* map, usize level);
void hashmap_free(HashMap* map, ItemFreer freer);

#endif
