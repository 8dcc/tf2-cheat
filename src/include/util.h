#ifndef UTIL_H_
#define UTIL_H_

#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include <link.h> /* link_map */
#include "sdk.h"

#define PRINT_BYTES(ptr, n)                        \
    {                                              \
        for (size_t i = 0; i < n; i++)             \
            printf("%X ", *((uint8_t*)(ptr) + i)); \
        putchar('\n');                             \
    }

/* Location of address + Size of offset + Offset */
#define RELATIVE2ABSOLUTE(addr) (void*)((void*)(addr) + 4 + *(uint32_t*)(addr))

/* Offset from dlopen() handler */
#define GET_OFFSET(HANDLER, OFFSET) \
    ((void*)(((struct link_map*)HANDLER)->l_addr) + OFFSET)

#define DEG2RAD(n) ((n)*M_PI / 180.0f)
#define RAD2DEG(n) ((n)*180.0f / M_PI)
#define CLAMP(val, min, max) \
    (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

/*----------------------------------------------------------------------------*/

void* get_interface(void* handle, const char* name);
size_t vmt_size(void* vmt);

vec3_t vec_add(vec3_t a, vec3_t b);
vec3_t vec_sub(vec3_t a, vec3_t b);
bool vec_cmp(vec3_t a, vec3_t b);
bool vec_is_zero(vec3_t v);
float vec_len2d(vec3_t v);
void vec_clamp(vec3_t v);
void vec_norm(vec3_t v);
vec3_t vec_to_ang(vec3_t v);
float angle_delta_rad(float a, float b);

bool protect_addr(void* ptr, int new_flags);

#endif /* UTIL_H_ */
