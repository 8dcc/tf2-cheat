#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdbool.h>
#include <string.h> /* CLONE_VMT: memcpy */
#include <stdlib.h> /* CLONE_VMT: malloc */
#include <SDL2/SDL.h>
#include "util.h"
#include "sdk.h"

/* Module names for handlers */
#define CLIENT_SO         "./tf/bin/client.so"
#define ENGINE_SO         "./bin/engine.so"
#define MATSURFACE_SO     "./bin/vguimatsurface.so"
#define VGUI_SO           "./bin/vgui2.so"
#define MATERIALSYSTEM_SO "./bin/materialsystem.so"
#define VSTDLIB_SO        "./bin/libvstdlib.so"
#define SDL_SO            "./bin/libSDL2-2.0.so.0"

/*----------------------------------------------------------------------------*/
/* Signatures */

/* See: https://github.com/8dcc/tf2-cheat/wiki/Getting-SDL-offsets */
#define SIG_StartDrawing \
    "F3 0F 2A C0 F3 0F 59 45 ? F3 0F 2C C0 89 85 ? ? ? ? E8 ? ? ? ? 8D 4D E4"

#define SIG_FinishDrawing \
    "89 04 24 FF 92 ? ? ? ? 89 34 24 E8 ? ? ? ? 80 7D 97 ? 0F 85 ? ? ? ?"

/* Only part in CL_Move where it sets bSendPacket to 1 */
#define SIG_bSendPacket                                                      \
    "BE ? ? ? ? E9 ? ? ? ? 8D B6 00 00 00 00 A1 ? ? ? ? C7 45 ? ? ? ? ? C7 " \
    "45 ? ? ? ? ? 85 C0 0F 84 ? ? ? ? 8D 55 A8 C7 44 24 ? ? ? ? ?"

/* CL_RegisterResources (Also in CL_ReadPackets) */
#define SIG_ClientState                                                       \
    "C7 04 24 ? ? ? ? E8 ? ? ? ? C7 04 24 ? ? ? ? 89 44 24 04 E8 ? ? ? ? A1 " \
    "? ? ? ?"

/* CConfirmDialog::GetResFile (Also in C_BasePlayer::GetToolRecordingState) */
#define SIG_CInput                                                            \
    "A1 ? ? ? ? 8B 10 89 04 24 FF 92 ? ? ? ? BA ? ? ? ? C9 84 C0 B8 ? ? ? ? " \
    "0F 44 D0 89 D0 C3"

/* CPrediction::RunCommand -> CPrediction::StartCommand -> SetPse..RandomSeed */
#define SIG_SetPredictionRandomSeed                                           \
    "75 7C 31 FF E8 ? ? ? ? 89 B3 ? ? ? ? 89 34 24 E8 ? ? ? ? 89 F8 89 1D ? " \
    "? ? ?"

/* CInput::CreateMove -> MD5_PseudoRandom */
#define SIG_MD5_PseudoRandom                                                   \
    "8B 45 08 F3 0F 11 80 ? ? ? ? 8B 45 0C 89 04 24 E8 ? ? ? ? 25 ? ? ? ? 89 " \
    "43 34 E8 ? ? ? ?"

/* CTargetID::GetTargetForSteamAvatar -> IsPlayerOnSteamFriendsList */
#define SIG_IsPlayerOnSteamFriendsList                                         \
    "55 89 E5 56 53 81 EC ? ? ? ? 65 A1 ? ? ? ? 89 45 F4 31 C0 8B 5D 0C E8 ? " \
    "? ? ? 85 C0 74 48 85 DB 74 44"

/*
 * NOTE: For commented version, see:
 *   https://github.com/8dcc/hl-cheat/blob/main/src/include/hooks.h
 *   https://github.com/8dcc/bms-cheat/blob/main/src/include/hooks.h
 *
 * prefix  | meaning
 * --------+-------------------------------
 * h_*     | handler ptr (global scope)
 * i_*     | interface ptr (global scope)
 * oVMTi_* | original vmt pointer (will be replaced with our own vmt)
 * nVMTi_* | new vmt pointer allocated by us
 * c_*     | class ptr with no VMT
 */
#define DECL_INTF(type, name)        \
    type* i_##name           = NULL; \
    VMT_##type* oVMTi_##name = NULL; \
    VMT_##type* nVMTi_##name = NULL;

#define DECL_INTF_EXTERN(type, name) \
    extern type* i_##name;           \
    extern VMT_##type* oVMTi_##name; \
    extern VMT_##type* nVMTi_##name;

#define DECL_CLASS(type, name) type* c_##name = NULL;

#define DECL_CLASS_EXTERN(type, name) extern type* c_##name;

#define DECL_SDL_FUNC(retType, name, ...)     \
    typedef retType (*name##_t)(__VA_ARGS__); \
    extern name##_t* name##Ptr;

#define CLONE_VMT(class, name)                                                \
    oVMT##name = name->vmt;                                                   \
    nVMT##name = malloc(vmt_size(name->vmt));                                 \
    if (!nVMT##name) {                                                        \
        fprintf(stderr, "CLONE_VMT: Could not allocate vmt for %s\n", #name); \
        return false;                                                         \
    }                                                                         \
    memcpy(nVMT##name, name->vmt, vmt_size(name->vmt));                       \
    name->vmt = nVMT##name;

#define RESTORE_VMT(class, name) \
    name->vmt = oVMT##name;      \
    free(nVMT##name);

/*----------------------------------------------------------------------------*/
/* Structs */

/* Indexes for each model in the g.model_index[] array */
enum model_indexes {
    /* Health */
    MDLIDX_MEDKIT_SMALL = 0,
    MDLIDX_MEDKIT_MEDIUM,
    MDLIDX_MEDKIT_LARGE,
    MDLIDX_MEDKIT_SMALL_BDAY,
    MDLIDX_MEDKIT_MEDIUM_BDAY,
    MDLIDX_MEDKIT_LARGE_BDAY,
    MDLIDX_PLATE,
    MDLIDX_PLATE_STEAK,
    MDLIDX_HALLOWEEN_MEDKIT_SMALL,
    MDLIDX_HALLOWEEN_MEDKIT_MEDIUM,
    MDLIDX_HALLOWEEN_MEDKIT_LARGE,
    MDLIDX_MUSHROOM_LARGE,

    /* Ammo */
    MDLIDX_AMMOPACK_SMALL,
    MDLIDX_AMMOPACK_MEDIUM,
    MDLIDX_AMMOPACK_LARGE,
    MDLIDX_AMMOPACK_LARGE_BDAY,
    MDLIDX_AMMOPACK_MEDIUM_BDAY,
    MDLIDX_AMMOPACK_SMALL_BDAY,

    /* Array size */
    MDLIDX_ARR_SZ,
};

typedef struct {
    const char* name; /* System name (From "fc-list" command) */
    int tall;
    int weight;
    int flags; /* EFontFlags */
    HFont id;  /* From ISurface::CreateFont() */
} font_t;

typedef struct {
    font_t main;
    font_t small;
    font_t tiny;
} font_list_t;

typedef struct {
    /* Index updated in LevelInitPostEntity */
    int localidx;
    int mdl_idx[MDLIDX_ARR_SZ];
    float sv_airaccelerate;
    float sv_maxspeed;
    float cl_forwardspeed;
    float cl_sidespeed;

    /* Updated in FrameStageNotify(FRAME_NET_UPDATE_END) */
    bool IsAlive;
    bool IsInGame;
    bool IsConnected;
    int MaxClients;
    int MaxEntities;
    VMatrix w2s_vmatrix;
    Entity* ents[2049];
    Entity* localplayer;
    Weapon* localweapon;

    /* Updated in EngineVGui::Paint() -> spectator_list() */
    bool spectated_1st;

    /* Updated in features and used in CreateMove */
    bool psilent;
} global_cache_t;

/*----------------------------------------------------------------------------*/
/* Global variables */

extern global_cache_t g;
extern font_list_t g_fonts;

extern bool* bSendPacket;

typedef void (*StartDrawing_t)(MatSurface*);
extern StartDrawing_t StartDrawing;
typedef void (*FinishDrawing_t)(MatSurface*);
extern FinishDrawing_t FinishDrawing;

typedef void (*SetPredictionRandomSeed_t)(usercmd_t*);
extern SetPredictionRandomSeed_t SetPredictionRandomSeed;
typedef int (*MD5_PseudoRandom_t)(int);
extern MD5_PseudoRandom_t MD5_PseudoRandom;

typedef bool (*IsPlayerOnSteamFriendsList_t)(Entity*, Entity*);
extern IsPlayerOnSteamFriendsList_t IsPlayerOnSteamFriendsList;

DECL_SDL_FUNC(void, SwapWindow, SDL_Window* window);
DECL_SDL_FUNC(int, PollEvent, SDL_Event* event);

DECL_INTF_EXTERN(BaseClient, baseclient);
DECL_INTF_EXTERN(EngineClient, engine);
DECL_INTF_EXTERN(EntityList, entitylist);
DECL_INTF_EXTERN(EngineVGui, enginevgui);
DECL_INTF_EXTERN(ICvar, cvar);
DECL_INTF_EXTERN(MatSurface, surface);
DECL_INTF_EXTERN(IPanel, panel);
DECL_INTF_EXTERN(IVModelInfo, modelinfo);
DECL_INTF_EXTERN(RenderView, renderview);
DECL_INTF_EXTERN(EngineTrace, enginetrace);
DECL_INTF_EXTERN(MaterialSystem, materialsystem);
DECL_INTF_EXTERN(ModelRender, modelrender);
DECL_INTF_EXTERN(GameMovement, gamemovement);
DECL_INTF_EXTERN(MoveHelper, movehelper);
DECL_INTF_EXTERN(IPrediction, prediction);
DECL_INTF_EXTERN(CInput, input);
DECL_INTF_EXTERN(ClientMode, clientmode);
DECL_CLASS_EXTERN(CGlobalVars, globalvars);
DECL_CLASS_EXTERN(CClientState, clientstate);

/*----------------------------------------------------------------------------*/
/* Functions from globals.c */

bool globals_init(void);
bool resore_vtables(void);
void fonts_init(void);

void cache_get_model_idx(void);
void cache_store_cvars(void);
void cache_reset_cvars(void);
void cache_reset(void);
void cache_update(void);

#endif /* GLOBALS_H_ */
