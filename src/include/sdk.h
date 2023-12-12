#ifndef SDK_H_
#define SDK_H_

#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>

#define STR(A, B) A##B
#define PADSTR(N) STR(pad, N)
#define PAD(N)    uint8_t PADSTR(__LINE__)[N]

#define METHOD(INSTANCE, METHOD) INSTANCE->vmt->METHOD(INSTANCE)
#define METHOD_ARGS(INSTANCE, METHOD, ...) \
    INSTANCE->vmt->METHOD(INSTANCE, __VA_ARGS__)

/*----------------------------------------------------------------------------*/
/* Forward declarations */

typedef struct TraceFilter TraceFilter;
typedef struct studiohdr_t studiohdr_t;
typedef struct model_t model_t;
typedef struct Renderable Renderable;
typedef struct Entity Entity;

typedef struct BaseClient BaseClient;
typedef struct EngineClient EngineClient;
typedef struct EntityList EntityList;
typedef struct EngineVGui EngineVGui;
typedef struct ConVar ConVar;
typedef struct ICvar ICvar;
typedef struct MatSurface MatSurface;
typedef struct IPanel IPanel;
typedef struct IVModelInfo IVModelInfo;
typedef struct EngineTrace EngineTrace;
typedef struct IMaterial IMaterial;
typedef struct IMatRenderContext IMatRenderContext;
typedef struct MaterialSystem MaterialSystem;
typedef struct ModelRender ModelRender;
typedef struct GameMovement GameMovement;
typedef struct MoveHelper MoveHelper;
typedef struct IPrediction IPrediction;
typedef struct RenderView RenderView;
typedef struct CInput CInput;
typedef struct ClientMode ClientMode;

/*----------------------------------------------------------------------------*/
/* Data structures and enums */

#include "sdk/enums.h"

typedef char byte;
typedef uint32_t CBaseHandle;
typedef CBaseHandle EntityHandle_t;
typedef unsigned int VPanel;

typedef struct {
    float x, y;
} vec2_t;

typedef struct {
    float x, y, z;
} vec3_t;

#define VEC_ZERO ((vec3_t){ 0.f, 0.f, 0.f })

typedef struct {
    float x, y, z, w;
} vec4_t;

typedef struct {
    float x, y, z, w;
} VectorAligned __attribute__((aligned(16)));

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
    uint8_t r, g, b, a; /* Range 0..255 */
} rgba_t;

typedef struct {
    float r, g, b, a; /* Range 0..1 */
} float_rgba_t;

typedef rgba_t Color;
typedef uint32_t HFont;

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

typedef struct {
    VectorAligned m_Start;
    VectorAligned m_Delta;
    VectorAligned m_StartOffset;
    VectorAligned m_Extents;
    bool m_IsRay;
    bool m_IsSwept;
} Ray_t;

typedef struct {
    vec3_t normal;
    float dist;
    uint8_t type;
    uint8_t signbits;
    uint8_t pad[2];
} cplane_t;

typedef struct {
    const char* name;
    int16_t surfaceProps;
    uint16_t flags;
} csurface_t;

typedef struct {
    /* CBaseTrace */
    vec3_t startpos;
    vec3_t endpos;
    cplane_t plane;
    float fraction;
    int contents;
    uint16_t dispFlags;
    bool allsolid;
    bool startsolid;

    /* CGameTrace */
    float fractionLeftSolid;
    csurface_t surface;
    int hit_group;
    int16_t physicsBone;
    struct Entity* entity;
    int hitbox;
} Trace_t;

typedef struct {
    bool (*ShouldHitEntity)(TraceFilter*, Entity* ent, int mask); /* 0 */
    int (*GetTraceType)(TraceFilter*);                            /* 1 */
} VMT_TraceFilter;

struct TraceFilter {
    VMT_TraceFilter* vmt;
    const struct Entity* skip;
};

typedef struct {
    studiohdr_t* m_pStudioHdr;
    void* m_pStudioHWData; /* studiohwdata_t */
    Renderable* m_pRenderable;
    const matrix3x4_t* m_pModelToWorld;
    void* m_decals; /* StudioDecalHandle_t */
    int m_drawFlags;
    int m_lod;
} DrawModelState_t;

typedef struct {
    vec3_t origin;
    vec3_t angles; /* QAngle */
    Renderable* pRenderable;
    const model_t* pModel;
    const matrix3x4_t* pModelToWorld;
    const matrix3x4_t* pLightingOffset;
    const vec3_t* pLightingOrigin;
    int flags;
    int entity_index;
    int skin;
    int body;
    int hitboxset;
    uint16_t instance; /* ModelInstanceHandle_t */
} ModelRenderInfo_t;

typedef struct {
    bool m_bFirstRunOfFunctions;
    bool m_bGameCodeMovedPlayer;
    EntityHandle_t m_nPlayerHandle;
    int m_nImpulseCommand;
    vec3_t m_vecViewAngles;    /* QAngle */
    vec3_t m_vecAbsViewAngles; /* QAngle */
    int m_nButtons;
    int m_nOldButtons;
    float m_flForwardMove;
    float m_flOldForwardMove;
    float m_flSideMove;
    float m_flUpMove;
    float m_flMaxSpeed;
    float m_flClientMaxSpeed;
    vec3_t m_vecVelocity;
    vec3_t m_vecAngles;    /* QAngle */
    vec3_t m_vecOldAngles; /* QAngle*/
    float m_outStepHeight;
    vec3_t m_outWishVel;
    vec3_t m_outJumpVel;
    vec3_t m_vecConstraintCenter;
    float m_flConstraintRadius;
    float m_flConstraintWidth;
    float m_flConstraintSpeedFactor;
    /* SetAbsOrigin() */
    /* GetAbsOrigin() */
    vec3_t m_vecAbsOrigin;
} CMoveData;

/* Make sure this struct is the right size. See CInput::GetUserCmd */
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
/* Classes */

#include "sdk/studiohdr.h"
#include "sdk/entity.h"
#include "sdk/weapon.h"
#include "sdk/convar.h"

/*----------------------------------------------------------------------------*/
/* Interfaces */

typedef struct {
    PAD(4 * 6);
    void (*LevelInitPostEntity)(BaseClient*); /* 6 */
    void (*LevelShutdown)(BaseClient*);       /* 7 */
    PAD(4 * 2);
    void (*HudProcessInput)(BaseClient*, bool bActive); /* 10 */
    void (*HudUpdate)(BaseClient*, bool bActive);       /* 11 */
    PAD(4 * 2);
    void (*IN_ActivateMouse)(BaseClient*); /* 14 */
    PAD(4 * 20);
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
    VMatrix* (*WorldToScreenMatrix)(EngineClient*); /* 36, not reliable */
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
    PAD(4 * 12);
    ConVar* (*FindVar)(ICvar*, const char* name);                     /* 12 */
    const ConVar* (*FindVar_const)(ICvar*, const char* name);         /* 13 */
    ConCommand* (*FindCommand)(ICvar*, const char* name);             /* 14 */
    const ConCommand* (*FindCommand_const)(ICvar*, const char* name); /* 15 */
} VMT_ICvar;

struct ICvar {
    VMT_ICvar* vmt;
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
    PAD(4 * 23);
    void (*DrawCircle)(MatSurface*, int x, int y, int radius,
                       int segments); /* 99 */
} VMT_MatSurface;

struct MatSurface {
    VMT_MatSurface* vmt;
};

typedef struct {
    PAD(4 * 37);
    const char* (*GetName)(IPanel*, VPanel); /* 37 */
    PAD(4 * 4);
    void (*PaintTraverse)(IPanel*, VPanel panel, bool forcerepaint,
                          bool allowforce); /* 42 */
} VMT_IPanel;

struct IPanel {
    VMT_IPanel* vmt;
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
    PAD(4 * 4);
    void (*TraceRay)(EngineTrace*, const Ray_t* ray, unsigned int fMask,
                     TraceFilter* pTraceFilter, Trace_t* pTrace); /* 4 */
} VMT_EngineTrace;

struct EngineTrace {
    VMT_EngineTrace* vmt;
};

typedef struct {
    PAD(4 * 27);
    void (*AlphaModulate)(IMaterial*, float alpha);               /* 27 */
    void (*ColorModulate)(IMaterial*, float r, float g, float b); /* 28 */
    void (*SetMaterialVarFlag)(IMaterial*, int flag, bool on);    /* 29 */
    bool (*GetMaterialVarFlag)(IMaterial*, int flag);             /* 30 */
} VMT_IMaterial;

struct IMaterial {
    VMT_IMaterial* vmt;
};

typedef struct {
    PAD(4 * 11);
    void (*DepthRange)(IMatRenderContext*, float zNear, float zFar);
} VMT_IMatRenderContext;

struct IMatRenderContext {
    VMT_IMatRenderContext* vmt;
};

typedef struct {
    PAD(4 * 73);
    IMaterial* (*FindMaterial)(MaterialSystem*, char const* pMaterialName,
                               const char* pTextureGroupName, bool complain,
                               const char* pComplainPrefix); /* 73 */
    PAD(4 * 26);
    IMatRenderContext* (*GetRenderContext)(MaterialSystem*); /* 100 */
} VMT_MaterialSystem;

struct MaterialSystem {
    VMT_MaterialSystem* vmt;
};

typedef struct {
    PAD(4 * 1);
    void (*ForcedMaterialOverride)(ModelRender*, IMaterial* newMaterial,
                                   int nOverrideType); /* 1 */
    PAD(4 * 17);
    void (*DrawModelExecute)(ModelRender*, const DrawModelState_t* state,
                             const ModelRenderInfo_t* pInfo,
                             matrix3x4_t* pCustomBoneToWorld); /* 19 */
} VMT_ModelRender;

struct ModelRender {
    VMT_ModelRender* vmt;
};

typedef struct {
    PAD(4 * 2);
    void (*ProcessMovement)(GameMovement*, Entity* pPlayer, CMoveData* pMove);
    void (*StartTrackPredictionErrors)(GameMovement*, Entity* pPlayer);  /* 3 */
    void (*FinishTrackPredictionErrors)(GameMovement*, Entity* pPlayer); /* 4 */
} VMT_GameMovement;

struct GameMovement {
    VMT_GameMovement* vmt;
};

typedef struct {
    void* unused; /* We only use the MoveHelper*, but never call any methods */
} VMT_MoveHelper;

struct MoveHelper {
    VMT_MoveHelper* vmt;
};

typedef struct {
    PAD(4 * 14);
    void (*SetLocalViewAngles)(IPrediction*, vec3_t* ang); /* 14 */
    PAD(4 * 3);
    void (*RunCommand)(IPrediction*, Entity* player, usercmd_t* cmd,
                       MoveHelper* helper); /* 18 */
    void (*SetupMove)(IPrediction*, Entity* player, usercmd_t* cmd,
                      MoveHelper* helper, CMoveData* move); /* 19 */
    void (*FinishMove)(IPrediction*, Entity* player, usercmd_t* cmd,
                       CMoveData* move); /* 20 */
} VMT_IPrediction;

struct IPrediction {
    VMT_IPrediction* vmt;
    int m_hLastGround; /* CHandle<C_BaseEntity> */
    bool m_bInPrediction;
    bool m_bFirstTimePredicted;
    bool m_bOldCLPredictValue;
    bool m_bEnginePaused;
    int m_nPreviousStartFrame;
    int m_nCommandsPredicted;
    int m_nServerCommandsAcknowledged;
    int m_bPreviousAckHadErrors;
    int m_nIncomingPacketNumber;
    float m_flIdealPitch;
};

typedef struct {
    PAD(4 * 4);
    void (*SetBlend)(RenderView*, float blend);                         /* 4 */
    float (*GetBlend)(RenderView*);                                     /* 5 */
    void (*SetColorModulation)(RenderView*, const float_rgba_t* blend); /* 6 */
    void (*GetColorModulation)(RenderView*, float_rgba_t* blend);       /* 7 */
    PAD(4 * 42);
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
    PAD(4 * 8);
    usercmd_t* (*GetUserCmd)(CInput*, int sequence_number); /* 8 */
} VMT_CInput;

struct CInput {
    VMT_CInput* vmt;
    PAD(0xF8);
    usercmd_t* m_pCommands; /* 0xFC */
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

typedef struct {
    PAD(0x10);
    void* m_NetChannel; /* INetChannel */
    PAD(0x18C);
    int m_nDeltaTick;
    PAD(0x110);
    int m_nMaxClients;
    PAD(0x4864); /* Was 0x486C, but chokedcommands offset was wrong */
    int lastoutgoingcommand;
    int chokedcommands;
    int last_command_ack;
} CClientState;

/*----------------------------------------------------------------------------*/
/* Functions from sdk.c */

void RayInit(Ray_t* ray, vec3_t start, vec3_t end);
void RayInitMinMax(Ray_t* ray, vec3_t start, vec3_t end, vec3_t min,
                   vec3_t max);

void TraceFilterInit(TraceFilter* filter, Entity* entity);
void TraceFilterInit_IgnoreFriendly(TraceFilter* filter, Entity* entity);

void TraceHull(vec3_t start, vec3_t end, vec3_t hull_min, vec3_t hull_max,
               uint32_t mask, Trace_t* trace);

bool IsBehindAndFacingTarget(Entity* owner, Entity* target);

#endif /* SDK_H_ */
