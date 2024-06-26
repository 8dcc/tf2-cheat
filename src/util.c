
#include <wchar.h>
#include <stdio.h>
#include <math.h>
#include <dlfcn.h>    /* dlsym */
#include <link.h>     /* link_map */
#include <unistd.h>   /* getpagesize */
#include <sys/mman.h> /* mprotect */

#include "include/sdk.h"
#include "include/util.h"
#include "include/math.h"
#include "include/globals.h"
#include "include/settings.h"

void* get_interface(void* handle, const char* name) {
    if (!handle) {
        ERR("Invalid handle for interface %s", name);
        return NULL;
    }

    typedef void* (*fn)(const char*, int*);
    const fn CreateInterface = (fn)dlsym(handle, "CreateInterface");

    /* dlsym failed */
    if (!CreateInterface) {
        ERR("dlsym() couldn't find CreateInterface");
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

/*----------------------------------------------------------------------------*/

bool can_shoot(void) {
    /* NOTE: g.localplayer and g.localweapon should be checked by the caller */
    /* NOTE: This should be called from prediction since curtime is changed
     * there. */

    /* If we are not in prediction, calculate the predicted curtime */
    const float curtime = (g.in_prediction) ? c_globalvars->curtime
                                            : g.localplayer->nTickBase *
                                                c_globalvars->interval_per_tick;

    return g.localplayer->flNextAttack <= curtime &&
           g.localweapon->flNextPrimaryAttack <= curtime;
}

bool melee_dealing_damage(usercmd_t* cmd) {
    if (METHOD(g.localweapon, GetWeaponId) == TF_WEAPON_KNIFE)
        return (cmd->buttons & IN_ATTACK) && can_shoot();

    /* We are done with the swing animation, and we are dealing damage on this
     * tick. Credits: SEOwned (and afaik to KGB as well) */
    return fabs(g.localweapon->smackTime - c_globalvars->curtime) <
           c_globalvars->interval_per_tick * 2.0f;
}

vec3_t center_of_hitbox(studiohdr_t* studio, matrix3x4_t* bonemat, int set,
                        int idx) {
    studiobbox_t* bbox = studiohdr_pHitbox(studio, set, idx);
    if (!bbox)
        return VEC_ZERO;

    vec3_t min, max;
    vec_transform(bbox->bbmin, &bonemat[bbox->bone], &min);
    vec_transform(bbox->bbmax, &bonemat[bbox->bone], &max);

    return (vec3_t){
        (min.x + max.x) * 0.5f,
        (min.y + max.y) * 0.5f,
        (min.z + max.z) * 0.5f,
    };
}

bool is_visible(vec3_t start, vec3_t end, Entity* target,
                bool ignore_friendly) {
    /* We initialize with a custom ShouldHitEntity() for ignoring teammates */
    TraceFilter filter;
    if (ignore_friendly)
        TraceFilterInit_IgnoreFriendly(&filter, g.localplayer);
    else
        TraceFilterInit(&filter, g.localplayer);

    Ray_t ray;
    RayInit(&ray, start, end);

    Trace_t trace;
    METHOD_ARGS(i_enginetrace, TraceRay, &ray, MASK_SHOT | CONTENTS_GRATE,
                &filter, &trace);

    return trace.entity == target || trace.fraction > 0.97f;
}

vec3_t velocity_to_ang(vec3_t vel) {
    if (vel.y == 0.f && vel.x == 0.f) {
        if (vel.z > 0.f)
            return (vec3_t){ 270.f, 0.f, 0.f };
        else
            return (vec3_t){ 90.f, 0.f, 0.f };
    }

    /* Similar to vec_to_ang() */
    float pitch = RAD2DEG(atan2f(-vel.z, vec_len(vel)));
    float yaw   = RAD2DEG(atan2f(vel.y, vel.x));

    if (pitch < 0.f)
        pitch += 360.f;

    if (yaw < 0.f)
        yaw += 360.f;

    return (vec3_t){ pitch, yaw, 0.f };
}

/* Jaw has (-180, 180) range, add an offset to that */
float add_offset_to_yaw(float jaw, float offset) {
    float ret = jaw + offset;

    if (ret > 180)
        ret = -180 + (ret - 180);

    return ret;
}

float sub_offset_to_yaw(float jaw, float offset) {
    float ret = jaw - offset;

    if (ret < -180)
        ret = 180 - (-180 - ret);

    return ret;
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

rgba_t hue2rgba(float h) {
    const float prime = fmodf(h / 60.f, 6);
    const float x     = 1 - fabsf(fmodf(prime, 2) - 1);

    rgba_t ret = { .r = 0, .g = 0, .b = 0, .a = 255 };

    if (prime >= 0 && prime < 1) {
        ret.r = 255;
        ret.g = (uint8_t)(x * 0xFF);
    } else if (prime < 2) {
        ret.g = 255;
        ret.r = (uint8_t)(x * 0xFF);
    } else if (prime < 3) {
        ret.g = 255;
        ret.b = (uint8_t)(x * 0xFF);
    } else if (prime < 4) {
        ret.b = 255;
        ret.g = (uint8_t)(x * 0xFF);
    } else if (prime < 5) {
        ret.b = 255;
        ret.r = (uint8_t)(x * 0xFF);
    } else if (prime < 6) {
        ret.r = 255;
        ret.b = (uint8_t)(x * 0xFF);
    }

    return ret;
}

struct nk_colorf get_team_color(int teamnum) {
    switch (teamnum) {
        case TEAM_RED:
            return settings.col_team_red;
        case TEAM_BLU:
            return settings.col_team_blu;
        default:
            /* Unknown team: gray */
            return (struct nk_colorf){ 0.63f, 0.63f, 0.63f, 1.f };
    }
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

void convert_player_name(char* dst, const char* src) {
    int dst_p = 0, src_p = 0;

    do {
        uint32_t c = src[src_p];

        if (c <= 0x7F) {
            dst[dst_p++] = c;
        } else {
            /* Multibyte char, print '?' and skip second byte.
             * TODO: Add multibyte char support for russian names, etc. */
            dst[dst_p++] = '?';

            if ((uint32_t)src[src_p + 1] > 0x7F)
                src_p++;
        }
    } while (src[src_p++] != '\0'); /* Exit when we copy '\0' */
}

/*----------------------------------------------------------------------------*/

/* clang-format off */
#define MUL_ROW(matrix, idx, vec) \
    (matrix.m[idx][0] * vec.x +   \
     matrix.m[idx][1] * vec.y +   \
     matrix.m[idx][2] * vec.z +   \
     matrix.m[idx][3])
/* clang-format on */

bool world_to_screen(vec3_t vec, vec2_t* screen) {
    if (vec_is_zero(vec))
        return false;

    float w = MUL_ROW(g.w2s_vmatrix, 3, vec);
    if (w < 0.01f)
        return false;

    int scr_w, scr_h;
    METHOD_ARGS(i_engine, GetScreenSize, &scr_w, &scr_h);

    screen->x = (scr_w / 2.0f) * (1.0f + MUL_ROW(g.w2s_vmatrix, 0, vec) / w);
    screen->y = (scr_h / 2.0f) * (1.0f - MUL_ROW(g.w2s_vmatrix, 1, vec) / w);

    return true;
}

/*----------------------------------------------------------------------------*/

#define BASE_HASH    5381
#define MAGIC_NUMBER 33

/*
 * Credits:
 *   - http://www.cse.yorku.ca/~oz/hash.html
 *     https://web.archive.org/web/20230714154454/http://www.cse.yorku.ca/~oz/hash.html
 *   - http://casualhacks.net/hashtool.html
 *     https://web.archive.org/web/20160102205932/http://www.casualhacks.net/hashtool.html
 *   - https://www.unknowncheats.me/forum/904026-post8.html
 *     https://web.archive.org/web/20140822095256/http://www.unknowncheats.me/forum/904026-post8.html
 */
uint32_t hash_str(const char* str) {
    uint32_t hashed = BASE_HASH;

    for (int i = 0; str[i] != '\0'; i++)
        hashed = (hashed * MAGIC_NUMBER) ^ str[i];

    return hashed;
}

/*----------------------------------------------------------------------------*/

bool protect_addr(void* ptr, int new_flags) {
    long page_size      = sysconf(_SC_PAGESIZE);
    long page_mask      = ~(page_size - 1);
    uintptr_t next_page = ((uintptr_t)ptr + page_size - 1) & page_mask;
    uintptr_t prev_page = next_page - page_size;
    void* page          = (void*)prev_page;

    if (mprotect(page, page_size, new_flags) == -1)
        return false;

    return true;
}
