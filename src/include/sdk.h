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
    void* vt;
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

#define METHOD(instance, method) instance->vt->method(instance)
#define METHOD_ARGS(instance, method, ...) \
    instance->vt->method(instance, __VA_ARGS__)

typedef struct BaseClient BaseClient;
typedef struct ClientMode ClientMode;

typedef struct {
    PAD(4 * 10);
    void (*HudProcessInput)(BaseClient*, bool bActive); /* 10 */
} VT_BaseClient;

struct BaseClient {
    VT_BaseClient* vt;
};

typedef struct {
    PAD(4 * 22);
    bool (*CreateMove)(ClientMode*, float flInputSampleTime, usercmd_t* cmd);
} VT_ClientMode;

struct ClientMode {
    VT_ClientMode* vt;
};

#endif /* SDK_H_ */
