#ifndef SDK_H_
#define SDK_H_

#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>

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
    float x, y, z, w;
} vec4_t;

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
typedef uint32_t HFont;

typedef struct {
    int sznameindex;
    int parent;
    int bonecontroller[6];
    vec3_t pos;
    vec4_t quat;
    vec3_t rot; /* RadianEuler */
    vec3_t posscale;
    vec3_t rotscale;
    matrix3x4_t poseToBone;
    vec4_t qAlignment;
    int flags;
    int proctype;
    int procindex;
    int physicsbone;
    int surfacepropidx;
    int contents;
    int unused[8];
} studiobone_t;

typedef struct {
    int id;
    int version;
    int checksum;
    /* pszName() */
    char name[64];
    int length;
    vec3_t eyeposition;
    vec3_t illumposition;
    vec3_t hull_min;
    vec3_t hull_max;
    vec3_t view_bbmin;
    vec3_t view_bbmax;
    int flags;
    int numbones;
    int boneindex;
    /* pBone(int i) */
    /* ... */
} studiohdr_t;

static inline studiobone_t* studiohdr_pBone(studiohdr_t* thisptr, const int i) {
    if ((i < 0) || (i > (thisptr->numbones - 1)))
        return NULL;

    return (studiobone_t*)(((void*)thisptr) + thisptr->boneindex) + i;
}

typedef struct {
    int x;
    int m_nUnscaledX;
    int y;
    int m_nUnscaledY;
    int width;
    int m_nUnscaledWidth;
    int height;
    int m_eStereoEye;
    int m_nUnscaledHeight;
    bool m_bOrtho;
    float m_OrthoLeft;
    float m_OrthoTop;
    float m_OrthoRight;
    float m_OrthoBottom;
    float fov;
    float fovViewmodel;
    vec3_t origin;
    vec3_t angles; /* QAngle */
    float zNear;
    float zFar;
    float zNearViewmodel;
    float zFarViewmodel;
    bool m_bRenderToSubrectOfLargerScreen;
    float m_flAspectRatio;
    bool m_bOffCenter;
    float m_flOffCenterTop;
    float m_flOffCenterBottom;
    float m_flOffCenterLeft;
    float m_flOffCenterRight;
    bool m_bDoBloomAndToneMapping;
    bool m_bCacheFullSceneState;
    bool m_bViewToProjectionOverride;
    VMatrix m_ViewToProjection;
} ViewSetup;

enum EFontFlags {
    FONTFLAG_NONE         = 0x000,
    FONTFLAG_ITALIC       = 0x001,
    FONTFLAG_UNDERLINE    = 0x002,
    FONTFLAG_STRIKEOUT    = 0x004,
    FONTFLAG_SYMBOL       = 0x008,
    FONTFLAG_ANTIALIAS    = 0x010,
    FONTFLAG_GAUSSIANBLUR = 0x020,
    FONTFLAG_ROTARY       = 0x040,
    FONTFLAG_DROPSHADOW   = 0x080,
    FONTFLAG_ADDITIVE     = 0x100,
    FONTFLAG_OUTLINE      = 0x200,
    FONTFLAG_CUSTOM       = 0x400,
    FONTFLAG_BITMAP       = 0x800,
};

/* "drawType" argument in ISurface::DrawPrintText */
typedef enum {
    FONT_DRAW_DEFAULT = 0,
    FONT_DRAW_NONADDITIVE,
    FONT_DRAW_ADDITIVE,
    FONT_DRAW_TYPE_COUNT = 2,
} FontDrawType_t;

/* "curStage" argument for IBaseClientDLL::FrameStageNotify */
typedef enum {
    FRAME_UNDEFINED = -1,
    FRAME_START,
    FRAME_NET_UPDATE_START,
    FRAME_NET_UPDATE_POSTDATAUPDATE_START,
    FRAME_NET_UPDATE_POSTDATAUPDATE_END,
    FRAME_NET_UPDATE_END,
    FRAME_RENDER_START,
    FRAME_RENDER_END,
} ClientFrameStage_t;

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
typedef struct IVModelInfo IVModelInfo;
typedef struct RenderView RenderView;
typedef struct ClientMode ClientMode;

typedef struct {
    PAD(4 * 6);
    void (*LevelInitPostEntity)(BaseClient*); /* 6 */
    void (*LevelShutdown)(BaseClient*);       /* 7 */
    PAD(4 * 2);
    void (*HudProcessInput)(BaseClient*, bool bActive); /* 10 */
    void (*HudUpdate)(BaseClient*, bool bActive);       /* 11 */
    PAD(4 * 23);
    void (*FrameStageNotify)(BaseClient*, ClientFrameStage_t curStage); /* 35 */
    PAD(4 * 23);
    bool (*GetPlayerView)(BaseClient*, ViewSetup* playerView); /* 59  */
} VMT_BaseClient;

struct BaseClient {
    VMT_BaseClient* vmt;
};

typedef struct {
    PAD(4 * 5);
    void (*GetScreenSize)(EngineClient*, int* w, int* h); /* 5 */
    PAD(4 * 2);
    bool (*GetPlayerInfo)(EngineClient*, int entId, player_info_t* i); /* 8 */
    PAD(4 * 3);
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
    PAD(4 * 4);
    int (*GetMaxEntities)(EntityList*); /* 9 */
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
    PAD(4 * 1);
    void (*SetTextFont)(MatSurface*, HFont font);                  /* 17 */
    void (*SetTextColor)(MatSurface*, int r, int g, int b, int a); /* 18 */
    PAD(4 * 1);
    void (*SetTextPos)(MatSurface*, int x, int y);   /* 20 */
    void (*GetTextPos)(MatSurface*, int* x, int* y); /* 21 */
    void (*PrintText)(MatSurface*, const wchar_t* text, int len,
                      FontDrawType_t drawType); /* 22 */
    PAD(4 * 29);
    void (*SetCursorAlwaysVisible)(MatSurface*, bool visible); /* 52 */
    PAD(4 * 13);
    HFont (*CreateFont)(MatSurface*); /* 66 */
    bool (*SetFontGlyphSet)(MatSurface*, HFont font, const char* sysfontname,
                            int tall, int weight, int blur, int scanlines,
                            int flags, int nRangeMin, int nRangeMax); /* 67 */
    PAD(4 * 7);
    void (*GetTextSize)(MatSurface*, HFont font, const wchar_t* str, int* w,
                        int* h); /* 75 */
} VMT_MatSurface;

struct MatSurface {
    VMT_MatSurface* vmt;
};

typedef struct {
    PAD(4 * 3);
    int (*GetModelIndex)(IVModelInfo*, const char* name); /* 3 */
    PAD(4 * 25);
    studiohdr_t* (*GetStudioModel)(IVModelInfo*, const model_t* mod); /* 29 */
} VMT_IVModelInfo;

struct IVModelInfo {
    VMT_IVModelInfo* vmt;
};

typedef struct {
    PAD(4 * 50);
    void (*GetMatricesForView)(RenderView*, const ViewSetup* view,
                               VMatrix* pWorldToView,
                               VMatrix* pViewToProjection,
                               VMatrix* pWorldToProjection,
                               VMatrix* pWorldToPixels); /* 50 */
} VMT_RenderView;

struct RenderView {
    VMT_RenderView* vmt;
};

typedef struct {
    PAD(4 * 22);
    bool (*CreateMove)(ClientMode*, float flInputSampleTime, usercmd_t* cmd);
} VMT_ClientMode;

struct ClientMode {
    VMT_ClientMode* vmt;
};

typedef struct {
    float realtime;
    int framecount;
    float absoluteframetime;
    float curtime;
    float frametime;
    int maxClients;
    int tickcount;
    float interval_per_tick;
    float interpolation_amount;
    int simTicksThisFrame;
    int network_protocol;
    void* pSaveData; /* CSaveRestoreData* */
    bool m_bClient;
    int nTimestampNetworkingBase;
    int nTimestampRandomizeWindow;
} CGlobalVars;

#endif /* SDK_H_ */
