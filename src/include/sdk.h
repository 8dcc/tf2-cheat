#ifndef SDK_H_
#define SDK_H_

#include <stdint.h>
#include <stdbool.h>

#define STR(a, b) a##b
#define PADSTR(n) STR(pad, n)
#define PAD(n)    uint8_t PADSTR(__LINE__)[n]

#define METHOD(instance, method) instance->vmt->method(instance)
#define METHOD_ARGS(instance, method, ...) \
    instance->vmt->method(instance, __VA_ARGS__)

/*----------------------------------------------------------------------------*/
/* Data structures and enums */

typedef char byte;

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
    float m[3][4];
} matrix3x4_t;

typedef struct {
    uint8_t r, g, b;
} rgb_t;

typedef struct {
    uint8_t r, g, b, a;
} rgba_t;

typedef rgba_t Color;

/* "mode" argument of EngineVGui::Paint */
enum paint_modes {
    PAINT_UIPANELS     = (1 << 0),
    PAINT_INGAMEPANELS = (1 << 1),
    PAINT_CURSOR       = (1 << 2),
};

/*----------------------------------------------------------------------------*/
/* Classes */

#include "sdk/usercmd_t.h"
#include "sdk/entity.h"
#include "sdk/weapon.h"

/*----------------------------------------------------------------------------*/
/* Interfaces */

typedef struct BaseClient BaseClient;
typedef struct EngineClient EngineClient;
typedef struct EntityList EntityList;
typedef struct EngineVGui EngineVGui;
typedef struct MatSurface MatSurface;
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
    int (*GetMaxClients)(EngineClient*);             /* 21 */
    PAD(4 * 4);
    bool (*IsInGame)(EngineClient*);    /* 26 */
    bool (*IsConnected)(EngineClient*); /* 27 */
    PAD(4 * 8);
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
    PAD(4 * 15);
    void (*Paint)(EngineVGui*, uint32_t mode); /* 15 */
} VMT_EngineVGui;

struct EngineVGui {
    VMT_EngineVGui* vmt;
};

typedef struct {
    PAD(4 * 10);
    void (*SetColor)(MatSurface*, int r, int g, int b, int a); /* 10 */
    PAD(4 * 1);
    void (*DrawFilledRect)(MatSurface*, int x0, int y0, int x1, int y1);
    PAD(4 * 1);
    void (*DrawRect)(MatSurface*, int x0, int y0, int x1, int y1); /* 14 */
    void (*DrawLine)(MatSurface*, int x0, int y0, int x1, int y1); /* 15 */
    PAD(4 * 36);
    void (*SetCursorAlwaysVisible)(MatSurface*, bool visible); /* 51 */
} VMT_MatSurface;

struct MatSurface {
    VMT_MatSurface* vmt;
};

typedef struct {
    PAD(4 * 22);
    bool (*CreateMove)(ClientMode*, float flInputSampleTime, usercmd_t* cmd);
} VMT_ClientMode;

struct ClientMode {
    VMT_ClientMode* vmt;
};

#endif /* SDK_H_ */
