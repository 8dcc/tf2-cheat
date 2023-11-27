#ifndef MATH_H_
#define MATH_H_ 1

#include <stdbool.h>
#include <stddef.h> /* size_t */
#include <float.h>  /* FLT_EPSILON */
#include <math.h>
#include "sdk.h"

#define ABS(X)     ((X) < 0 ? -(X) : (X))
#define MAX(A, B)  ((A) < (B) ? B : A)
#define MIN(A, B)  ((A) > (B) ? B : A)
#define DEG2RAD(N) ((N)*M_PI / 180.0F)
#define RAD2DEG(N) ((N)*180.0F / M_PI)
#define CLAMP(VAL, MIN, MAX) \
    (((VAL) > (MAX)) ? (MAX) : (((VAL) < (MIN)) ? (MIN) : (VAL)))

#define VEC_COPY(DST, SRC) \
    (DST).x = (SRC).x;     \
    (DST).y = (SRC).y;     \
    (DST).z = (SRC).z;

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

static inline float vec_dist(vec3_t a, vec3_t b) {
    return vec_len(vec_sub(a, b));
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

#endif /* MATH_H_ */
