#ifndef UTIL_H_
#define UTIL_H_ 1

#include <stdbool.h>
#include <stddef.h> /* size_t */
#include <link.h>   /* link_map */
#include "sdk.h"

#define ERR(...)                                  \
    do {                                          \
        fprintf(stderr, "enoch: %s: ", __func__); \
        fprintf(stderr, __VA_ARGS__);             \
        fputc('\n', stderr);                      \
    } while (0)

#define PRINT_BYTES(PTR, N)                        \
    do {                                           \
        printf("%p: ", PTR);                       \
        for (size_t i = 0; i < N; i++) {           \
            if (*((uint8_t*)(PTR) + i) < 0x10)     \
                putchar('0');                      \
            printf("%X ", *((uint8_t*)(PTR) + i)); \
        }                                          \
        putchar('\n');                             \
    } while (0)

#define VEC_PRINT(VEC) \
    printf("%s: (%3.2f, %3.2f, %3.2f)\n", #VEC, (VEC).x, (VEC).y, (VEC).z)

/* Location of address + Size of offset + Offset */
#define RELATIVE2ABSOLUTE(ADDR) (void*)((void*)(ADDR) + 4 + *(uint32_t*)(ADDR))

/* Offset from dlopen() handler */
#define GET_OFFSET(HANDLER, OFFSET) \
    ((void*)(((struct link_map*)HANDLER)->l_addr) + OFFSET)

#define LENGTH(ARR) (sizeof(ARR) / sizeof(ARR[0]))

/* nk_colorf -> rgba_t */
#define NK2COL(COLORF)                                               \
    ((rgba_t){ (COLORF).r * 255, (COLORF).g * 255, (COLORF).b * 255, \
               (COLORF).a * 255 })

/* rgba_t -> nk_colorf */
#define COL2NK(COLOR)                                          \
    ((struct nk_colorf){ (COLOR).r / 255.f, (COLOR).g / 255.f, \
                         (COLOR).b / 255.f, (COLOR).a / 255.f })

/*----------------------------------------------------------------------------*/

void* get_interface(void* handle, const char* name);
size_t vmt_size(void* vmt);
void* find_sig(const char* module, const byte* ida_pattern);

bool can_shoot(void);
bool melee_dealing_damage(usercmd_t* cmd);
vec3_t center_of_hitbox(studiohdr_t* studio, matrix3x4_t* bonemat, int set,
                        int idx);
vec3_t velocity_to_ang(vec3_t vel);
float add_offset_to_yaw(float jaw, float offset);
float sub_offset_to_yaw(float jaw, float offset);

rgba_t col_scale(rgba_t c, float factor);
void draw_text(int x, int y, bool center, HFont f, rgba_t c, const char* str);
void get_text_size(HFont f, const char* str, int* w, int* h);
void convert_player_name(char* dst, const char* src);

bool world_to_screen(vec3_t vec, vec2_t* screen);

uint32_t hash_str(const char* str);

bool protect_addr(void* ptr, int new_flags);

#endif /* UTIL_H_ */
