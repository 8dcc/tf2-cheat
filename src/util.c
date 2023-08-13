
#include <wchar.h>
#include <stdio.h>
#include <math.h>
#include <dlfcn.h>    /* dlsym */
#include <link.h>     /* link_map */
#include <unistd.h>   /* getpagesize */
#include <sys/mman.h> /* mprotect */

#include "include/sdk.h"
#include "include/util.h"
#include "include/globals.h"

void* get_interface(void* handle, const char* name) {
    if (!handle) {
        fprintf(stderr, "get_interface: invalid handle for interface %s\n",
                name);
        return NULL;
    }

    typedef void* (*fn)(const char*, int*);
    const fn CreateInterface = (fn)dlsym(handle, "CreateInterface");

    /* dlsym failed */
    if (!CreateInterface) {
        fprintf(stderr, "get_interface: dlsym couldn't get CreateInterface\n");
        return NULL;
    }

    return CreateInterface(name, NULL);
}

size_t vmt_size(void* vmt) {
    /* Pointer to vmt -> Array of function pointers */
    void** funcs = (void**)vmt;

    int i = 0;

    while (funcs[i])
        i++;

    /* Return bytes, not number of function pointers */
    return i * sizeof(void*);
}

void* find_sig(const char* module, const byte* pattern) {
    struct our_link_map {
        /* Base from link.h */
        ElfW(Addr) l_addr;
        const char* l_name;
        ElfW(Dyn) * l_ld;
        struct our_link_map* l_next;
        struct our_link_map* l_prev;

        /* Added */
        struct our_link_map* real;
        long int ns;
        struct libname_list* moduleName;
        ElfW(Dyn) * info[DT_NUM + DT_VERSIONTAGNUM + DT_EXTRANUM + DT_VALNUM +
                         DT_ADDRNUM];
        const ElfW(Phdr) * phdr;
    };

    struct our_link_map* link = dlopen(module, RTLD_NOLOAD | RTLD_NOW);
    if (!link) {
        fprintf(stderr, "find_sig: can't open module \"%s\"\n", module);
        return NULL;
    }

    byte* start = (byte*)link->l_addr;
    byte* end   = start + link->phdr[0].p_memsz;

    dlclose(link);

    const byte* memPos = start;
    const byte* patPos = pattern;

    /* Iterate memory area until *patPos is '\0' (we found pattern).
     * If we start a pattern match, keep checking all pattern positions until we
     * are done or until mismatch. If we find mismatch, reset pattern position
     * and continue checking at the memory location where we started +1 */
    while (memPos < end && *patPos != '\0') {
        if (*memPos == *patPos || *patPos == '?') {
            memPos++;
            patPos++;
        } else {
            start++;
            memPos = start;
            patPos = pattern;
        }
    }

    /* We reached end of pattern, we found it */
    if (*patPos == '\0')
        return start;

    return NULL;
}

/*----------------------------------------------------------------------------*/

vec3_t vec_add(vec3_t a, vec3_t b) {
    return (vec3_t){
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z,
    };
}

vec3_t vec_sub(vec3_t a, vec3_t b) {
    return (vec3_t){
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z,
    };
}

bool vec_cmp(vec3_t a, vec3_t b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool vec_is_zero(vec3_t v) {
    return v.x == 0.0f && v.y == 0.0f && v.z == 0.0f;
}

float vec_len2d(vec3_t v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

void vec_clamp(vec3_t* v) {
    v->x = CLAMP(v->x, -89.0f, 89.0f);
    v->y = CLAMP(remainderf(v->y, 360.0f), -180.0f, 180.0f);
    v->z = CLAMP(v->z, -50.0f, 50.0f);
}

void vec_norm(vec3_t* v) {
    v->x = isfinite(v->x) ? remainder(v->x, 360) : 0;
    v->y = isfinite(v->y) ? remainder(v->y, 360) : 0;
    v->z = 0.0f;
}

void vec_transform(vec3_t v, matrix3x4_t* mat, vec3_t* out) {
    out->x = dot_product(v, *(vec3_t*)mat->m[0]) + mat->m[0][3];
    out->y = dot_product(v, *(vec3_t*)mat->m[1]) + mat->m[1][3];
    out->z = dot_product(v, *(vec3_t*)mat->m[2]) + mat->m[2][3];
}

vec3_t vec_to_ang(vec3_t v) {
    return (vec3_t){
        .x = RAD2DEG(atan2(-v.z, hypot(v.x, v.y))),
        .y = RAD2DEG(atan2(v.y, v.x)),
        .z = 0.0f,
    };
}

vec3_t ang_to_vec(vec3_t a) {
    float sy = sin(a.y / 180.f * (float)(M_PI));
    float cy = cos(a.y / 180.f * (float)(M_PI));

    float sp = sin(a.x / 180.f * (float)(M_PI));
    float cp = cos(a.x / 180.f * (float)(M_PI));

    return (vec3_t){
        .x = cp * cy,
        .y = cp * sy,
        .z = -sp,
    };
}

float angle_delta_rad(float a, float b) {
    float delta = isfinite(a - b) ? remainder(a - b, 360) : 0;

    if (a > b && delta >= M_PI)
        delta -= M_PI * 2;
    else if (delta <= -M_PI)
        delta += M_PI * 2;

    return delta;
}

/*----------------------------------------------------------------------------*/

bool IsBehindAndFacingTarget(Entity* target) {
    if (!g.IsAlive)
        return false;

    /* Get a vector from owner origin to target origin */
    vec3_t vecToTarget;
    vecToTarget   = vec_sub(*METHOD(target, WorldSpaceCenter),
                            *METHOD(g.localplayer, WorldSpaceCenter));
    vecToTarget.z = 0.0f;
    vec_norm(&vecToTarget);

    /* Get owner forward view vector */
    vec3_t vecOwnerForward = ang_to_vec(METHOD(g.localplayer, EyeAngles));
    vecOwnerForward.z      = 0.0f;
    vec_norm(&vecOwnerForward);

    /* Get target forward view vector */
    vec3_t vecTargetForward = ang_to_vec(METHOD(target, EyeAngles));
    vecTargetForward.z      = 0.0f;
    vec_norm(&vecTargetForward);

    /* Make sure owner is behind, facing and aiming at target's back */
    float flPosVsTargetViewDot = dot_product(vecToTarget, vecTargetForward);
    float flPosVsOwnerViewDot  = dot_product(vecToTarget, vecOwnerForward);
    float flViewAnglesDot      = dot_product(vecTargetForward, vecOwnerForward);

    return (flPosVsTargetViewDot > 0.f && flPosVsOwnerViewDot > 0.5 &&
            flViewAnglesDot > -0.3f);
}

/*----------------------------------------------------------------------------*/

rgba_t col_scale(rgba_t c, float factor) {
    if (factor < 1.0f)
        return (rgba_t){
            .r = (int)MIN(255, (float)c.r * factor),
            .g = (int)MIN(255, (float)c.g * factor),
            .b = (int)MIN(255, (float)c.b * factor),
            .a = c.a,
        };
    else if (factor > 1.0f)
        return (rgba_t){
            .r = (int)MIN(255, c.r + (float)(255 - c.r) * (factor - 1.f)),
            .g = (int)MIN(255, c.g + (float)(255 - c.g) * (factor - 1.f)),
            .b = (int)MIN(255, c.b + (float)(255 - c.b) * (factor - 1.f)),
            .a = c.a,
        };
    else
        return c;
}

void draw_text(int x, int y, bool center, HFont f, rgba_t c, const char* str) {
    static wchar_t wstr[512] = { '\0' };
    swprintf(wstr, 511, L"%hs", (char*)str);

    if (center) {
        int w = 0, h = 0;
        METHOD_ARGS(i_surface, GetTextSize, f, wstr, &w, &h);

        x -= (w / 2);
    }

    METHOD_ARGS(i_surface, SetTextPos, x, y);
    METHOD_ARGS(i_surface, SetTextFont, f);
    METHOD_ARGS(i_surface, SetTextColor, c.r, c.g, c.b, c.a);
    METHOD_ARGS(i_surface, PrintText, wstr, wcslen(wstr), FONT_DRAW_DEFAULT);
}

void get_text_size(HFont f, const char* str, int* w, int* h) {
    static wchar_t wstr[512] = { '\0' };
    swprintf(wstr, 511, L"%hs", (char*)str);

    METHOD_ARGS(i_surface, GetTextSize, f, wstr, w, h);
}

/*----------------------------------------------------------------------------*/

/* clang-format off */
#define MUL_ROW(matrix, idx, vec) \
    (matrix->m[idx][0] * vec.x +  \
     matrix->m[idx][1] * vec.y +  \
     matrix->m[idx][2] * vec.z +  \
     matrix->m[idx][3])
/* clang-format on */

bool world_to_screen(vec3_t vec, vec2_t* screen) {
    if (vec_is_zero(vec))
        return false;

    /* Get viewmatrix */
    const VMatrix* matrix = METHOD(i_engine, WorldToScreenMatrix);
    if (!matrix)
        return false;

    float w = MUL_ROW(matrix, 3, vec);
    if (w < 0.01f)
        return false;

    int scr_w, scr_h;
    METHOD_ARGS(i_engine, GetScreenSize, &scr_w, &scr_h);

    screen->x = (scr_w / 2.0f) * (1.0f + MUL_ROW(matrix, 0, vec) / w);
    screen->y = (scr_h / 2.0f) * (1.0f - MUL_ROW(matrix, 1, vec) / w);

    return true;
}

/*----------------------------------------------------------------------------*/

#define PAGE_SIZE          getpagesize()
#define PAGE_MASK          (~(PAGE_SIZE - 1))
#define PAGE_ALIGN(x)      ((x + PAGE_SIZE - 1) & PAGE_MASK)
#define PAGE_ALIGN_DOWN(x) (PAGE_ALIGN(x) - PAGE_SIZE)

bool protect_addr(void* ptr, int new_flags) {
    void* p  = (void*)PAGE_ALIGN_DOWN((int)ptr);
    int pgsz = getpagesize();

    if (mprotect(p, pgsz, new_flags) == -1) {
        printf("hl-cheat: error protecting %p\n", ptr);
        return false;
    }

    return true;
}
