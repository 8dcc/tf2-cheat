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
    float m[4][4];
} VMatrix;

typedef struct {
    uint8_t r, g, b;
} rgb_t;

typedef struct {
    uint8_t r, g, b, a;
} rgba_t;

typedef rgba_t Color;

/*----------------------------------------------------------------------------*/
/* Classes */

#include "sdk/usercmd_t.h"
#include "sdk/entity.h"

/*----------------------------------------------------------------------------*/
/* Interfaces */

#define METHOD(instance, method) instance->vmt->method(instance)
#define METHOD_ARGS(instance, method, ...) \
    instance->vmt->method(instance, __VA_ARGS__)

typedef struct BaseClient BaseClient;
typedef struct EngineClient EngineClient;
typedef struct EntityList EntityList;
typedef struct ClientMode ClientMode;

typedef struct {
    PAD(4 * 10);
    void (*HudProcessInput)(BaseClient*, bool bActive); /* 10 */
} VMT_BaseClient;

struct BaseClient {
    VMT_BaseClient* vmt;
};

typedef struct {
    PAD(4 * 5);
    void (*GetScreenSize)(EngineClient*, int* w, int* h); /* 5 */
    PAD(4 * 6);
    int (*GetLocalPlayer)(EngineClient*); /* 12 */
    PAD(4 * 6);
    void (*GetViewAngles)(EngineClient*, vec3_t* v); /* 19 */
    void (*SetViewAngles)(EngineClient*, vec3_t* v); /* 20 */
    PAD(4 * 15);
    VMatrix* (*WorldToScreenMatrix)(EngineClient*); /* 36 */
} VMT_EngineClient;

struct EngineClient {
    VMT_EngineClient* vmt;
};

typedef struct {
    PAD(4 * 3);
    Entity* (*GetClientEntity)(EntityList*, int entnum); /* 4 */
} VMT_EntityList;

struct EntityList {
    VMT_EntityList* vmt;
};

typedef struct {
    PAD(4 * 22);
    bool (*CreateMove)(ClientMode*, float flInputSampleTime, usercmd_t* cmd);
} VMT_ClientMode;

struct ClientMode {
    VMT_ClientMode* vmt;
};

#endif /* SDK_H_ */