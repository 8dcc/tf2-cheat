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

uint32_t netvars_get_offset_recursive(struct hashmap* netvars, int amount, ...);

#define NETVAR(amount, ...)                                               \
    static uint32_t netvar_offset = NULL;                                 \
    if (!netvar_offset) {                                                 \
        netvar_offset =                                                   \
          netvars_get_offset_recursive(g_netvars, amount, ##__VA_ARGS__); \
    }

#define NETVAR_RETURN(self, type, amount, ...) \
    NETVAR(amount, __VA_ARGS__)                \
    return *(type*)((uint32_t)self + netvar_offset)

#define NETVAR_SET(self, type, value, amount, ...) \
    NETVAR(amount, __VA_ARGS__)                    \
    *(type*)((uint32_t)self + netvar_offset) = value

#endif /* NETVARS_H_ */