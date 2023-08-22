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
#define MATERIALSYSTEM_SO "./bin/materialsystem.so"
#define SDL_SO            "./bin/libSDL2-2.0.so.0"

/* Signatures */
#define SIG_StartDrawing                                                    \
    "\xF3\x0F\x2A\xC0\xF3\x0F\x59\x45?\xF3\x0F\x2C\xC0\x89\x85????\xE8????" \
    "\x8D\x4D\xE4"

#define SIG_FinishDrawing \
    "\x89\x04\x24\xFF\x92????\x89\x34\x24\xE8????\x80\x7D\x97?\x0F\x85????"

#define SIG_bSendPacket                                                        \
    "\xBE????\xE9????\x8D\xB6\x00\x00\x00\x00\xA1????\xC7\x45?????\xC7\x45???" \
    "??\x85\xC0\x0F\x84????\x8D\x55\xA8\xC7\x44\x24?????"

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

    /* Updated in FrameStageNotify(FRAME_NET_UPDATE_END) */
    bool IsAlive;
    bool IsInGame;
    bool IsConnected;
    int MaxClients;
    int MaxEntities;
    VMatrix w2s_vmatrix;
    Entity* ents[2049];
    Entity* localplayer;

    /* Updated in EngineVGui::Paint() -> spectator_list() */
    bool spectated_1st;
} global_cache_t;

/*----------------------------------------------------------------------------*/
/* Global variables */

extern void* h_client;
extern void* h_engine;
extern void* h_matsurface;
extern void* h_materialsystem;
extern void* h_sdl2;

extern global_cache_t g;
extern font_list_t g_fonts;

extern bool* bSendPacket;

typedef void (*StartDrawing_t)(MatSurface*);
extern StartDrawing_t StartDrawing;
typedef void (*FinishDrawing_t)(MatSurface*);
extern FinishDrawing_t FinishDrawing;

DECL_SDL_FUNC(void, SwapWindow, SDL_Window* window);
DECL_SDL_FUNC(int, PollEvent, SDL_Event* event);

DECL_INTF_EXTERN(BaseClient, baseclient);
DECL_INTF_EXTERN(EngineClient, engine);
DECL_INTF_EXTERN(EntityList, entitylist);
DECL_INTF_EXTERN(EngineVGui, enginevgui);
DECL_INTF_EXTERN(MatSurface, surface);
DECL_INTF_EXTERN(IVModelInfo, modelinfo);
DECL_INTF_EXTERN(RenderView, renderview);
DECL_INTF_EXTERN(EngineTrace, enginetrace);
DECL_INTF_EXTERN(MaterialSystem, materialsystem);
DECL_INTF_EXTERN(ModelRender, modelrender);
DECL_INTF_EXTERN(ClientMode, clientmode);
DECL_CLASS_EXTERN(CGlobalVars, globalvars);

/*----------------------------------------------------------------------------*/
/* Functions from globals.c */

bool globals_init(void);
bool resore_vtables(void);
void fonts_init(void);

void cache_get_model_idx(void);
void cache_reset(void);
void cache_update(void);
void update_w2s_viewmatrix(void);

#endif /* GLOBALS_H_ */
