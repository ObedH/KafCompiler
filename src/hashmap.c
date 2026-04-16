#include "hashmap.h"

#include <stdio.h>
#include "hash.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define HASHMAP_CAPACITY 16


HashMap* hashmap_create(void) {
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    if (!map) {
        fprintf(stderr, "Failed to mallocate memory for HashMap\n");
        return NULL;
    }
    map->capacity = HASHMAP_CAPACITY;
    map->size = 0;
    map->entries = (HashMapEntry*)calloc(HASHMAP_CAPACITY, sizeof(HashMapEntry));
    if (!map->entries) {
        fprintf(stderr, "Failed to mallocate memory for HashMap entries\n");
	free(map);
        return NULL;
    }
    return map;
}

void hashmap_put(HashMap* map, const char* key, void* value) {
    if(!value) {
        return;
    }
    if(!key) {
        fprintf(stderr, "NULL KEY!\n");
        return;
    }
    u64 hash = hash_str(key);
    usize index = hash % map->capacity;

    for (usize i = 0; i < map->capacity; i++) {
        usize probe_index = (index + i) % map->capacity;
        HashMapEntry* entry = &map->entries[probe_index];

        if (entry->key == NULL) {
            entry->key = malloc(strlen(key) + 1);
            if (!entry->key) {
                fprintf(stderr, "Failed to allocate memory for hashmap key\n");
                return;
            }
            strcpy(entry->key, key);
            entry->value = value;
            map->size++;
            return;
        }
        if (strcmp(entry->key, key) == 0) {
            entry->value = value; // Update existing key
            return;
        }
    }
    fprintf(stderr, "HashMap is full, cannot insert new key: %s\n", key);
}

void* hashmap_get(HashMap* map, const char* key) {
    if (!key) {
        return NULL;
    }
    u64 hash = hash_str(key);
    usize index = hash % map->capacity;

    for (usize i = 0; i < map->capacity; i++) {
        usize probe_index = (index + i) % map->capacity;
        HashMapEntry* entry = &map->entries[probe_index];

        if (entry->key == NULL) {
            return NULL; // Key not found
        }
        if (strcmp(entry->key, key) == 0) {
            return entry->value; // Key found
        }
    }
    return NULL; // Key not found
}

void hashmap_put_raw(HashMap* map, u64 key, void* value) {
    hashmap_put(map, (const char*)(uintptr_t)key, value);
}
void* hashmap_get_raw(HashMap* map, u64 key) {
    return hashmap_get(map, (const char*)(uintptr_t)key);
}

void hashmap_free(HashMap* map, ItemFreer freer) {
    if (!map) {
        return;
    }
    for(usize i = 0; i < map->capacity; i ++) {
        if(map->entries[i].key) {
            free(map->entries[i].key);
        }
        if(freer && map->entries[i].value) {
            freer(map->entries[i].value);
        }
    }
    if (map->entries) {
	free(map->entries);
    }
    free(map);
}
