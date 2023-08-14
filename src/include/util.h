#ifndef UTIL_H_
#define UTIL_H_

#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include <link.h> /* link_map */
#include "sdk.h"

#define PRINT_BYTES(ptr, n)                        \
    {                                              \
        for (size_t i = 0; i < n; i++) {           \
            if (*((uint8_t*)(ptr) + i) < 0x10)     \
                putchar('0');                      \
            printf("%X ", *((uint8_t*)(ptr) + i)); \
        }                                          \
        putchar('\n');                             \
    }

/* Location of address + Size of offset + Offset */
#define RELATIVE2ABSOLUTE(addr) (void*)((void*)(addr) + 4 + *(uint32_t*)(addr))

/* Offset from dlopen() handler */
#define GET_OFFSET(HANDLER, OFFSET) \
    ((void*)(((struct link_map*)HANDLER)->l_addr) + OFFSET)

#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))
#define MAX(a, b)   ((a) < (b) ? b : a)
#define MIN(a, b)   ((a) > (b) ? b : a)
#define DEG2RAD(n)  ((n)*M_PI / 180.0f)
#define RAD2DEG(n)  ((n)*180.0f / M_PI)
#define CLAMP(val, min, max) \
    (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

/* nk_colorf -> rgba_t */
#define NK2COL(COLORF)                                               \
    ((rgba_t){ (COLORF).r * 255, (COLORF).g * 255, (COLORF).b * 255, \
               (COLORF).a * 255 })

/*----------------------------------------------------------------------------*/

void* get_interface(void* handle, const char* name);
size_t vmt_size(void* vmt);
void* find_sig(const char* module, const byte* pattern);

vec3_t vec_add(vec3_t a, vec3_t b);
vec3_t vec_sub(vec3_t a, vec3_t b);
bool vec_cmp(vec3_t a, vec3_t b);
bool vec_is_zero(vec3_t v);
float vec_len2d(vec3_t v);
void vec_clamp(vec3_t* v);
void vec_norm(vec3_t* v);
void vec_transform(vec3_t v, matrix3x4_t* mat, vec3_t* out);
vec3_t vec_to_ang(vec3_t v);
vec3_t ang_to_vec(vec3_t a);
float angle_delta_rad(float a, float b);
#define dot_product(a, b) ((a).x * (b).x + (a).y * (b).y + (a).z * (b).z)

bool IsBehindAndFacingTarget(Entity* target);

rgba_t col_scale(rgba_t c, float factor);
void draw_text(int x, int y, bool center, HFont f, rgba_t c, const char* str);
void get_text_size(HFont f, const char* str, int* w, int* h);
void convert_player_name(char* dst, const char* src);

bool world_to_screen(vec3_t vec, vec2_t* screen);

bool protect_addr(void* ptr, int new_flags);

#endif /* UTIL_H_ */
