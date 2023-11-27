
#include <stdio.h>
#include <dlfcn.h>
#include <sys/mman.h> /* PROT_* */
#include "include/math.h"
#include "include/globals.h"

/* See wiki */
#define SWAPWINDOW_OFFSET 0xFD648
#define POLLEVENT_OFFSET  0xFCF64

#define GET_HANDLER(VAR, STR)                         \
    void* VAR = dlopen(STR, RTLD_LAZY | RTLD_NOLOAD); \
    if (!VAR) {                                       \
        ERR("Can't open " #VAR);                      \
        return false;                                 \
    }

#define GET_INTERFACE(TYPE, VAR, HANDLER, STR) \
    VAR = (TYPE)get_interface(HANDLER, STR);   \
    if (!VAR || !VAR->vmt) {                   \
        ERR("Can't load " #VAR);               \
        return false;                          \
    }

#define GET_SIGNATURE(VAR, MODULE, SIG)            \
    void* VAR = find_sig(MODULE, SIG);             \
    if (!VAR) {                                    \
        ERR("Coundn't match signature for " #SIG); \
        return false;                              \
    }

/*----------------------------------------------------------------------------*/

/* Global cache and fonts */
global_cache_t g;
font_list_t g_fonts;

/* Signature pointers */
bool* bSendPacket = NULL;

/* Signature functions */
StartDrawing_t StartDrawing                             = NULL;
FinishDrawing_t FinishDrawing                           = NULL;
SetPredictionRandomSeed_t SetPredictionRandomSeed       = NULL;
MD5_PseudoRandom_t MD5_PseudoRandom                     = NULL;
IsPlayerOnSteamFriendsList_t IsPlayerOnSteamFriendsList = NULL;

/* SDL functions */
SwapWindow_t* SwapWindowPtr = NULL;
PollEvent_t* PollEventPtr   = NULL;

/* Interfaces and classes
 * NOTE: Macro defined in globals.h */
DECL_INTF(BaseClient, baseclient);
DECL_INTF(EngineClient, engine);
DECL_INTF(EntityList, entitylist);
DECL_INTF(EngineVGui, enginevgui);
DECL_INTF(MatSurface, surface);
DECL_INTF(IPanel, panel);
DECL_INTF(IVModelInfo, modelinfo);
DECL_INTF(RenderView, renderview);
DECL_INTF(EngineTrace, enginetrace);
DECL_INTF(MaterialSystem, materialsystem);
DECL_INTF(ModelRender, modelrender);
DECL_INTF(GameMovement, gamemovement);
DECL_INTF(MoveHelper, movehelper);
DECL_INTF(IPrediction, prediction);
DECL_INTF(ClientMode, clientmode);
DECL_CLASS(CClientState, clientstate);
DECL_CLASS(CGlobalVars, globalvars);

/*----------------------------------------------------------------------------*/

/* See: https://github.com/8dcc/bms-cheat/wiki */
static inline ClientMode* get_clientmode(void) {
    const int byte_offset = 1;

    void* func_ptr      = i_baseclient->vmt->HudProcessInput;
    void* g_pClientMode = *(void**)(func_ptr + byte_offset); /* 60 1F 06 02 */
    ClientMode* ret     = *(ClientMode**)g_pClientMode;

    return ret;
}

/* Same as clientmode but with a different function and offset */
static inline CGlobalVars* get_globalvars(void) {
    const int byte_offset = 9;

    void* func_ptr   = i_baseclient->vmt->HudUpdate;
    void* gpGlobals  = *(void**)(func_ptr + byte_offset); /* 70 33 F9 01 */
    CGlobalVars* ret = *(CGlobalVars**)gpGlobals;

    return ret;
}

static inline bool get_sigs(void) {
    /* NOTE: Signature scanning and pointer functions can be a bit messy. Keep
     * in mind that RELATIVE2ABSOLUTE() dereferences the pointer once */

    /* MatSurface functions */
    GET_SIGNATURE(pat_StartDrawing, MATSURFACE_SO, SIG_StartDrawing);
    StartDrawing = RELATIVE2ABSOLUTE(pat_StartDrawing + 20);

    GET_SIGNATURE(pat_FinishDrawing, MATSURFACE_SO, SIG_FinishDrawing);
    FinishDrawing = RELATIVE2ABSOLUTE(pat_FinishDrawing + 13);

    /* CL_Move's bSendPacket
     * NOTE: We set PROT_WRITE since since we will change the pointer value */
    GET_SIGNATURE(pat_bSendPacket, ENGINE_SO, SIG_bSendPacket);
    bSendPacket = pat_bSendPacket + 1;
    protect_addr(bSendPacket, PROT_READ | PROT_WRITE | PROT_EXEC);

    /* ClientState
     * NOTE: We don't use RELATIVE2ABSOLUTE() since it's an absolute address */
    GET_SIGNATURE(pat_ClientState, ENGINE_SO, SIG_ClientState);
    c_clientstate = *(CClientState**)(pat_ClientState + 3);

    /* CBaseEntity::SetPredictionRandomSeed() */
    GET_SIGNATURE(pat_SetPredictionRandomSeed, CLIENT_SO,
                  SIG_SetPredictionRandomSeed);
    SetPredictionRandomSeed =
      RELATIVE2ABSOLUTE(pat_SetPredictionRandomSeed + 19);

    /* MD5_PseudoRandom() */
    GET_SIGNATURE(pat_MD5_PseudoRandom, CLIENT_SO, SIG_MD5_PseudoRandom);
    MD5_PseudoRandom = RELATIVE2ABSOLUTE(pat_MD5_PseudoRandom + 18);

    /* IsPlayerOnSteamFriendsList()
     * NOTE: We don't use RELATIVE2ABSOLUTE() and we don't add any offset since
     * this is the signature to the function itself. */
    GET_SIGNATURE(pat_IsPlayerOnSteamFriendsList, CLIENT_SO,
                  SIG_IsPlayerOnSteamFriendsList);
    IsPlayerOnSteamFriendsList = pat_IsPlayerOnSteamFriendsList;

    return true;
}

bool globals_init(void) {
    /* Handlers */
    GET_HANDLER(h_client, CLIENT_SO);
    GET_HANDLER(h_engine, ENGINE_SO);
    GET_HANDLER(h_matsurface, MATSURFACE_SO);
    GET_HANDLER(h_vgui, VGUI_SO);
    GET_HANDLER(h_materialsystem, MATERIALSYSTEM_SO);
    GET_HANDLER(h_sdl2, SDL_SO);

    /* SDL2 */
    SwapWindowPtr = (SwapWindow_t*)GET_OFFSET(h_sdl2, SWAPWINDOW_OFFSET);
    PollEventPtr  = (PollEvent_t*)GET_OFFSET(h_sdl2, POLLEVENT_OFFSET);

    /* Interfaces */
    GET_INTERFACE(BaseClient*, i_baseclient, h_client, "VClient017");
    GET_INTERFACE(EngineClient*, i_engine, h_engine, "VEngineClient014");
    GET_INTERFACE(EntityList*, i_entitylist, h_client, "VClientEntityList003");
    GET_INTERFACE(EngineVGui*, i_enginevgui, h_engine, "VEngineVGui002");
    GET_INTERFACE(MatSurface*, i_surface, h_matsurface, "VGUI_Surface030");
    GET_INTERFACE(IPanel*, i_panel, h_vgui, "VGUI_Panel009");
    GET_INTERFACE(IVModelInfo*, i_modelinfo, h_engine, "VModelInfoClient006");
    GET_INTERFACE(RenderView*, i_renderview, h_engine, "VEngineRenderView014");
    GET_INTERFACE(EngineTrace*, i_enginetrace, h_engine,
                  "EngineTraceClient003");
    GET_INTERFACE(MaterialSystem*, i_materialsystem, h_materialsystem,
                  "VMaterialSystem081");
    GET_INTERFACE(ModelRender*, i_modelrender, h_engine, "VEngineModel016");
    GET_INTERFACE(GameMovement*, i_gamemovement, h_client, "GameMovement001");
    GET_INTERFACE(IPrediction*, i_prediction, h_client, "VClientPrediction001");

    /* Other interfaces */
    i_clientmode = get_clientmode();
    if (!i_clientmode || !i_clientmode->vmt) {
        ERR("Couldn't load i_clientmodebms");
        return false;
    }

    c_globalvars = get_globalvars();
    if (!c_globalvars) {
        ERR("Couldn't load c_globalvars");
        return false;
    }

    /* Needed for write permission on the VMTs. Macro declared in globals.h */
    CLONE_VMT(BaseClient, i_baseclient);
    CLONE_VMT(ClientMode, i_clientmode);
    CLONE_VMT(EngineVGui, i_enginevgui);
    CLONE_VMT(IPanel, i_panel);
    CLONE_VMT(ModelRender, i_modelrender);
    CLONE_VMT(IPrediction, i_prediction);

    dlclose(h_client);
    dlclose(h_engine);
    dlclose(h_matsurface);
    dlclose(h_vgui);
    dlclose(h_materialsystem);
    dlclose(h_sdl2);

    /* Individual functions/globals from signatures */
    if (!get_sigs())
        return false;

    /* Initialize global cache */
    cache_reset();
    cache_update();
    if (g.IsInGame) {
        /* Call stuff that should be run each level change when injecting */
        g.localidx = METHOD(i_engine, GetLocalPlayer);
        cache_get_model_idx();
    }

    return true;
}

bool resore_vtables(void) {
    /* Restore original VTables when unloading. Macro declared in globals.h */
    RESTORE_VMT(BaseClient, i_baseclient);
    RESTORE_VMT(ClientMode, i_clientmode);
    RESTORE_VMT(EngineVGui, i_enginevgui);
    RESTORE_VMT(IPanel, i_panel);
    RESTORE_VMT(ModelRender, i_modelrender);
    RESTORE_VMT(IPrediction, i_prediction);

    return true;
}

/*----------------------------------------------------------------------------*/

#define CREATE_FONT(FONT)                                                 \
    if (!METHOD_ARGS(i_surface, SetFontGlyphSet, FONT.id, FONT.name,      \
                     FONT.tall, FONT.weight, 0, 0, FONT.flags, 0, 0)) {   \
        fprintf(stderr,                                                   \
                "WARNING: fonts_init: couldn't create font \"%s\" using " \
                "default monospace.\n",                                   \
                FONT.name);                                               \
        FONT.id = 16;                                                     \
    }

void fonts_init(void) {
    /* Initialize font_t structs */
    g_fonts.main = (font_t){
        .name   = "CozetteVector",
        .tall   = 15,
        .weight = 700,
        .flags  = FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS,
        .id     = METHOD(i_surface, CreateFont),
    };
    g_fonts.small = (font_t){
        .name   = "CozetteVector",
        .tall   = 14,
        .weight = 700,
        .flags  = FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS,
        .id     = METHOD(i_surface, CreateFont),
    };
    g_fonts.tiny = (font_t){
        .name   = "CozetteVector",
        .tall   = 13, /* Digits are a bit weird */
        .weight = 700,
        .flags  = FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS,
        .id     = METHOD(i_surface, CreateFont),
    };

    /* Create fonts with the data */
    CREATE_FONT(g_fonts.main);
    CREATE_FONT(g_fonts.small);
    CREATE_FONT(g_fonts.tiny);
}

/*----------------------------------------------------------------------------*/

#define STORE_MDL(arr_idx, mdl_str) \
    g.mdl_idx[MDLIDX_##arr_idx] =   \
      METHOD_ARGS(i_modelinfo, GetModelIndex, mdl_str);

void cache_get_model_idx(void) {
    /* Health */
    STORE_MDL(MEDKIT_SMALL, "models/items/medkit_small.mdl");
    STORE_MDL(MEDKIT_MEDIUM, "models/items/medkit_medium.mdl");
    STORE_MDL(MEDKIT_LARGE, "models/items/medkit_large.mdl");
    STORE_MDL(MEDKIT_SMALL_BDAY, "models/items/medkit_small_bday.mdl");
    STORE_MDL(MEDKIT_MEDIUM_BDAY, "models/items/medkit_medium_bday.mdl");
    STORE_MDL(MEDKIT_LARGE_BDAY, "models/items/medkit_large_bday.mdl");
    STORE_MDL(PLATE, "models/items/plate.mdl");
    STORE_MDL(PLATE_STEAK, "models/items/plate_steak.mdl");
    STORE_MDL(HALLOWEEN_MEDKIT_SMALL, "models/props_halloween/"
                                      "halloween_medkit_small.mdl");
    STORE_MDL(HALLOWEEN_MEDKIT_MEDIUM, "models/props_halloween/"
                                       "halloween_medkit_medium.mdl");
    STORE_MDL(HALLOWEEN_MEDKIT_LARGE, "models/props_halloween/"
                                      "halloween_medkit_large.mdl");
    STORE_MDL(MUSHROOM_LARGE, "models/items/ld1/mushroom_large.mdl");

    /* Ammo */
    STORE_MDL(AMMOPACK_SMALL, "models/items/ammopack_small.mdl");
    STORE_MDL(AMMOPACK_MEDIUM, "models/items/ammopack_medium.mdl");
    STORE_MDL(AMMOPACK_LARGE, "models/items/ammopack_large.mdl");
    STORE_MDL(AMMOPACK_LARGE_BDAY, "models/items/ammopack_large_bday.mdl");
    STORE_MDL(AMMOPACK_MEDIUM_BDAY, "models/items/ammopack_medium_bday.mdl");
    STORE_MDL(AMMOPACK_SMALL_BDAY, "models/items/ammopack_small_bday.mdl");
}

void cache_reset(void) {
    g.IsInGame    = false;
    g.IsConnected = false;
    g.IsAlive     = false;
    g.MaxClients  = 0;
    g.MaxEntities = 0;

    g.localplayer = NULL;
    g.localweapon = NULL;
    for (int i = 0; i < (int)LENGTH(g.ents); i++)
        g.ents[i] = NULL;
}

void cache_update(void) {
    g.IsInGame    = METHOD(i_engine, IsInGame);
    g.IsConnected = METHOD(i_engine, IsConnected);

    if (g.IsInGame) {
        g.MaxClients  = METHOD(i_engine, GetMaxClients);
        g.MaxEntities = METHOD(i_entitylist, GetMaxEntities);

        /* Store localplayer even if not alive */
        g.localplayer = METHOD_ARGS(i_entitylist, GetClientEntity, g.localidx);
        if (g.localplayer) {
            g.IsAlive = METHOD(g.localplayer, IsAlive);

            if (g.IsAlive)
                g.localweapon = METHOD(g.localplayer, GetWeapon);
        }

        /* First iterate players */
        for (int i = 1; i <= g.MaxClients; i++) {
            Entity* ent      = METHOD_ARGS(i_entitylist, GetClientEntity, i);
            Networkable* net = GetNetworkable(ent);

            if (!ent || METHOD(net, IsDormant) || !METHOD(ent, IsAlive))
                continue;

            g.ents[i] = ent;
        }

        /* Then other entities */
        const int last_entity = MIN((int)LENGTH(g.ents) - 1, g.MaxEntities);
        for (int i = g.MaxClients + 1; i < last_entity; i++) {
            Entity* ent      = METHOD_ARGS(i_entitylist, GetClientEntity, i);
            Networkable* net = GetNetworkable(ent);

            if (!ent || METHOD(net, IsDormant))
                continue;

            g.ents[i] = ent;
        }
    }
}
