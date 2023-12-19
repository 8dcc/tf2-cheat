
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

/* "E0" -> 224 */
uint8_t hex_to_byte(const char* hex) {
    int ret = 0;

    /* Skip leading spaces, if any */
    while (*hex == ' ')
        hex++;

    /* Store a byte (two digits of string) */
    for (int i = 0; i < 2 && hex[i] != '\0'; i++) {
        char c = hex[i];

        /* For example "E ", although the format should always be "0E" */
        if (c == ' ')
            break;

        uint8_t n = 0;
        if (c >= '0' && c <= '9')
            n = c - '0';
        else if (c >= 'a' && c <= 'f')
            n = 10 + c - 'a';
        else if (c >= 'A' && c <= 'F')
            n = 10 + c - 'A';

        /* Shift size of 0xF and add the next half of byte */
        ret <<= 4;
        ret |= n & 0xF;
    }

    return ret & 0xFF;
}

/* See: https://8dcc.github.io/programming/signature-scanning.html */
void* find_sig(const char* module, const char* pattern) {
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
        ERR("Can't open module \"%s\"", module);
        return NULL;
    }

    uint8_t* start = (uint8_t*)link->l_addr;
    uint8_t* end   = start + link->phdr[0].p_memsz;

    dlclose(link);

    /* Skip preceding spaces from pattern, if any */
    while (*pattern == ' ')
        pattern++;

    /* Current position in memory and current position in pattern */
    uint8_t* mem_ptr    = start;
    const char* pat_ptr = pattern;

    /* Iterate until we reach the end of the memory or the end of the pattern */
    while (mem_ptr < end && *pat_ptr != '\0') {
        /* Wildcard, always match */
        if (*pat_ptr == '?') {
            mem_ptr++;
            pat_ptr++;

            /* Remove trailing spaces after '?'
             * NOTE: I reused this code, but you could use `goto` */
            while (*pat_ptr == ' ')
                pat_ptr++;

            continue;
        }

        /* "E0" -> 224 */
        uint8_t cur_byte = hex_to_byte(pat_ptr);

        if (*mem_ptr == cur_byte) {
            /* Found exact byte match in sequence, go to next byte in memory */
            mem_ptr++;

            /* Go to next byte separator in pattern (space) */
            while (*pat_ptr != ' ' && *pat_ptr != '\0')
                pat_ptr++;
        } else {
            /* Byte didn't match, check pattern from the begining on the next
             * position in memory */
            start++;
            mem_ptr = start;
            pat_ptr = pattern;
        }

        /* Skip trailing spaces */
        while (*pat_ptr == ' ')
            pat_ptr++;
    }

    /* If we reached end of pattern, return the match. Otherwise, NULL */
    return (*pat_ptr == '\0') ? start : NULL;
}

/*----------------------------------------------------------------------------*/

bool can_shoot(void) {
    /* NOTE: g.localplayer and g.localweapon should be checked by the caller */
    /* NOTE: This should be called from prediction since curtime is changed
     * there. */
    /* TODO: Add g.in_prediction, if outside pred, calculate curtime locally. */
    return g.localplayer->flNextAttack <= c_globalvars->curtime &&
           g.localweapon->flNextPrimaryAttack <= c_globalvars->curtime;
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

#define PAGE_MASK          (~(PAGE_SZ - 1))
#define PAGE_ALIGN(x)      ((x + PAGE_SZ - 1) & PAGE_MASK)
#define PAGE_ALIGN_DOWN(x) (PAGE_ALIGN(x) - PAGE_SZ)

bool protect_addr(void* ptr, int new_flags) {
    int PAGE_SZ = getpagesize();
    void* p     = (void*)PAGE_ALIGN_DOWN((uint32_t)ptr);

    if (mprotect(p, PAGE_SZ, new_flags) == -1) {
        ERR("Error protecting %p", ptr);
        return false;
    }

    return true;
}
