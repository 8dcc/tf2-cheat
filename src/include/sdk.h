#ifndef SDK_H_
#define SDK_H_

#include <stdint.h>
#include <stdbool.h>

#define STR(a, b) a##b
#define PADSTR(n) STR(pad, n)
#define PAD(n)    uint8_t PADSTR(__LINE__)[n]

/*----------------------------------------------------------------------------*/
/* Data structures and enums */

typedef struct {
    float x, y;
} vec2_t;

typedef struct {
    float x, y, z;
} vec3_t;

typedef struct {
    uint8_t r, g, b;
} rgb_t;

typedef struct {
    uint8_t r, g, b, a;
} rgba_t;

typedef rgba_t Color;

typedef struct {
    void* vmt;
    int command_number;
    int tick_count;
    vec3_t viewangles;
    float forwardmove;
    float sidemove;
    float upmove;
    int buttons;
    uint8_t impulse;
    int weaponselect;
    int weaponsubtype;
    int random_seed;
    short mousedx;
    short mousedy;
    bool hasbeenpredicted;
} usercmd_t;

/*----------------------------------------------------------------------------*/
/* Interfaces */

#define METHOD(instance, method) instance->vmt->method(instance)
#define METHOD_ARGS(instance, method, ...) \
    instance->vmt->method(instance, __VA_ARGS__)

typedef struct BaseClient BaseClient;
typedef struct ClientMode ClientMode;

typedef struct {
    PAD(4 * 10);
    void (*HudProcessInput)(BaseClient*, bool bActive); /* 10 */
} VMT_BaseClient;

struct BaseClient {
    VMT_BaseClient* vmt;
};

typedef struct {
    PAD(4 * 22);
    bool (*CreateMove)(ClientMode*, float flInputSampleTime, usercmd_t* cmd);
} VMT_ClientMode;

struct ClientMode {
    VMT_ClientMode* vmt;
};

#endif /* SDK_H_ */
