#ifndef UTIL_H_
#define UTIL_H_

#include <stdbool.h>
#include <stddef.h> /* size_t */
#include <float.h>  /* FLT_EPSILON */
#include <math.h>
#include <link.h> /* link_map */
#include "sdk.h"

#define ERR(...)                                  \
    do {                                          \
        fprintf(stderr, "enoch: %s: ", __func__); \
        fprintf(stderr, __VA_ARGS__);             \
        fputc('\n', stderr);                      \
    } while (0)

#define PRINT_BYTES(ptr, n)                        \
    do {                                           \
        for (size_t i = 0; i < n; i++) {           \
            if (*((uint8_t*)(ptr) + i) < 0x10)     \
                putchar('0');                      \
            printf("%X ", *((uint8_t*)(ptr) + i)); \
        }                                          \
        putchar('\n');                             \
    } while (0)

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

#define VEC_COPY(dst, src) \
    (dst).x = (src).x;     \
    (dst).y = (src).y;     \
    (dst).z = (src).z;

/* nk_colorf -> rgba_t */
#define NK2COL(COLORF)                                               \
    ((rgba_t){ (COLORF).r * 255, (COLORF).g * 255, (COLORF).b * 255, \
               (COLORF).a * 255 })

/* rgba_t -> nk_colorf */
#define COL2NK(COLOR)                                          \
    ((struct nk_colorf){ (COLOR).r / 255.f, (COLOR).g / 255.f, \
                         (COLOR).b / 255.f, (COLOR).a / 255.f })

/*----------------------------------------------------------------------------*/

static inline float vec_dotproduct(vec3_t a, vec3_t b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

static inline vec3_t vec_add(vec3_t a, vec3_t b) {
    return (vec3_t){
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z,
    };
}

static inline vec3_t vec_sub(vec3_t a, vec3_t b) {
    return (vec3_t){
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z,
    };
}

static inline vec3_t vec_mul(vec3_t a, vec3_t b) {
    return (vec3_t){
        .x = a.x * b.x,
        .y = a.y * b.y,
        .z = a.z * b.z,
    };
}

static inline vec3_t vec_flmul(vec3_t a, float b) {
    return (vec3_t){
        .x = a.x * b,
        .y = a.y * b,
        .z = a.z * b,
    };
}

static inline bool vec_equal(vec3_t a, vec3_t b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

static inline bool vec_is_zero(vec3_t v) {
    return v.x == 0.0f && v.y == 0.0f && v.z == 0.0f;
}

static inline float vec_len(vec3_t v) {
    return sqrtf(vec_dotproduct(v, v));
}

static inline float vec_len2d(vec3_t v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

static inline void ang_clamp(vec3_t* v) {
    v->x = CLAMP(v->x, -89.0f, 89.0f);
    v->y = CLAMP(remainderf(v->y, 360.0f), -180.0f, 180.0f);
    v->z = CLAMP(v->z, -50.0f, 50.0f);
}

static inline void vec_norm(vec3_t* v) {
    v->x = isfinite(v->x) ? remainderf(v->x, 360.f) : 0.f;
    v->y = isfinite(v->y) ? remainderf(v->y, 360.f) : 0.f;
    v->z = 0.0f;
}

static inline void vec_norm_in_place(vec3_t* v) {
    float len        = vec_len(*v);
    float len_normal = 1.f / (FLT_EPSILON + len);

    v->x *= len_normal;
    v->y *= len_normal;
    v->z *= len_normal;
}

static inline void vec_transform(vec3_t v, matrix3x4_t* mat, vec3_t* out) {
    out->x = vec_dotproduct(v, *(vec3_t*)mat->m[0]) + mat->m[0][3];
    out->y = vec_dotproduct(v, *(vec3_t*)mat->m[1]) + mat->m[1][3];
    out->z = vec_dotproduct(v, *(vec3_t*)mat->m[2]) + mat->m[2][3];
}

static inline vec3_t vec_to_ang(vec3_t v) {
    return (vec3_t){
        .x = RAD2DEG(atan2(-v.z, hypot(v.x, v.y))),
        .y = RAD2DEG(atan2(v.y, v.x)),
        .z = 0.0f,
    };
}

static inline vec3_t ang_to_vec(vec3_t a) {
    const float sy = sin(a.y / 180.f * (float)(M_PI));
    const float cy = cos(a.y / 180.f * (float)(M_PI));

    const float sp = sin(a.x / 180.f * (float)(M_PI));
    const float cp = cos(a.x / 180.f * (float)(M_PI));

    return (vec3_t){
        .x = cp * cy,
        .y = cp * sy,
        .z = -sp,
    };
}

static inline float angle_delta_rad(float a, float b) {
    float delta = isfinite(a - b) ? remainder(a - b, 360) : 0;

    if (a > b && delta >= M_PI)
        delta -= M_PI * 2;
    else if (delta <= -M_PI)
        delta += M_PI * 2;

    return delta;
}

static inline float vec_length_sqr(vec3_t v) {
    return ((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

/*----------------------------------------------------------------------------*/

void* get_interface(void* handle, const char* name);
size_t vmt_size(void* vmt);
void* find_sig(const char* module, const byte* pattern);

bool can_shoot(void);
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
