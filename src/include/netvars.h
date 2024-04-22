#ifndef NETVARS_H_
#define NETVARS_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "../dependencies/hashmap/hashmap.h"

/* Get number of string __VA_ARGS__. If used somewhere else, we should move it
 * to util.h */
#define VA_ARGS_NUM(...) (sizeof((char*[]){ __VA_ARGS__ }) / sizeof(char*))

#define NETVAR(...)                                            \
    static int32_t netvar_offset = -1;                         \
    if (netvar_offset < 0) {                                   \
        netvar_offset = netvars_get_offset_recursive(          \
          g_netvars, VA_ARGS_NUM(__VA_ARGS__), ##__VA_ARGS__); \
    }

#define NETVAR_RETURN(SELF, TYPE, ...) \
    NETVAR(__VA_ARGS__)                \
    return *(TYPE*)((uintptr_t)SELF + netvar_offset)

#define NETVAR_SET(SELF, TYPE, VALUE, ...) \
    NETVAR(__VA_ARGS__)                    \
    *(TYPE*)((uintptr_t)SELF + netvar_offset) = VALUE

/*----------------------------------------------------------------------------*/

typedef struct {
    uint32_t offset;
    char* name;
    Hashmap* nodes;
} node_t;

/*----------------------------------------------------------------------------*/

/* Defined in netvars.c */
extern Hashmap* g_netvars;

Hashmap* netvars_init();
void netvars_dump(Hashmap* netvars);
void netvars_free(Hashmap* netvars);
bool netvars_get(Hashmap* netvars, const char* name, node_t** netvar);

uint32_t netvars_get_offset_recursive(Hashmap* netvars, int amount, ...);

#endif /* NETVARS_H_ */
