#ifndef NETVARS_H_
#define NETVARS_H_

#include <stdint.h>
#include <stdbool.h>
#include "../dependencies/hashmap/hashmap.h"

typedef struct {
    uint32_t offset;
    char* name;
    struct hashmap* nodes;
} node_t;

struct hashmap* netvars_init();
void netvars_dump(struct hashmap* netvars);
void netvars_free(struct hashmap* netvars);
bool netvars_get(struct hashmap* netvars, const char* name, node_t** netvar);

/* TODO: Extern it inside global.h */
extern struct hashmap* g_netvars;

#endif /* NETVARS_H_ */