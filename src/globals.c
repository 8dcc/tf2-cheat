
#include <stdio.h>
#include <dlfcn.h>
#include "include/globals.h"

/* See wiki */
#define SWAPWINDOW_OFFSET 0xFD648
#define POLLEVENT_OFFSET  0xFCF64

#define GET_HANDLER(VAR, STR)                                   \
    VAR = dlopen(STR, RTLD_LAZY | RTLD_NOLOAD);                 \
    if (!VAR) {                                                 \
        fprintf(stderr, "globals_init: can't open " #VAR "\n"); \
        return false;                                           \
    }

#define GET_INTERFACE(TYPE, VAR, HANDLER, STR)                  \
    VAR = (TYPE)get_interface(HANDLER, STR);                    \
    if (!VAR || !VAR->vmt) {                                    \
        fprintf(stderr, "globals_init: can't load " #VAR "\n"); \
        return false;                                           \
    }

#define GET_PATTERN(VAR, MODULE, SIG)                                      \
    void* VAR = find_sig(MODULE, SIG);                                     \
    if (!VAR) {                                                            \
        fprintf(stderr, "get_sigs: coundn't find pattern for " #SIG "\n"); \
        return false;                                                      \
    }

/*----------------------------------------------------------------------------*/

void* h_client     = NULL;
void* h_engine     = NULL;
void* h_matsurface = NULL;
void* h_sdl2       = NULL;

global_cache_t g;
font_list_t g_fonts;

StartDrawing_t StartDrawing   = NULL;
FinishDrawing_t FinishDrawing = NULL;

SwapWindow_t* SwapWindowPtr = NULL;
PollEvent_t* PollEventPtr   = NULL;

DECL_INTF(BaseClient, baseclient);
DECL_INTF(EngineClient, engine);
DECL_INTF(EntityList, entitylist);
DECL_INTF(EngineVGui, enginevgui);
DECL_INTF(MatSurface, surface);
DECL_INTF(IVModelInfo, modelinfo);
DECL_INTF(ClientMode, clientmode);

/*----------------------------------------------------------------------------*/

/* See: https://github.com/8dcc/bms-cheat/wiki */
static inline ClientMode* get_clientmode(void) {
    const int byte_offset = 1;

    void* func_ptr      = i_baseclient->vmt->HudProcessInput;
    void* g_pClientMode = *(void**)(func_ptr + byte_offset); /* 60 1F 06 02 */
    ClientMode* ret     = *(ClientMode**)g_pClientMode;

    return ret;
}

static inline bool get_sigs(void) {
    GET_PATTERN(pat_StartDrawing, MATSURFACE_SO, SIG_StartDrawing);
    StartDrawing = RELATIVE2ABSOLUTE(pat_StartDrawing + 20);

    GET_PATTERN(pat_FinishDrawing, MATSURFACE_SO, SIG_FinishDrawing);
    FinishDrawing = RELATIVE2ABSOLUTE(pat_FinishDrawing + 13);

    return true;
}

bool globals_init(void) {
    /* Handlers */
    GET_HANDLER(h_client, CLIENT_SO);
    GET_HANDLER(h_engine, ENGINE_SO);
    GET_HANDLER(h_matsurface, MATSURFACE_SO);
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
    GET_INTERFACE(IVModelInfo*, i_modelinfo, h_engine, "VModelInfoClient006");

    /* Other interfaces */
    i_clientmode = get_clientmode();
    if (!i_clientmode || !i_clientmode->vmt) {
        fprintf(stderr, "globals_init: couldn't load i_clientmodebms\n");
        return false;
    }

    /* Needed for write permission on the VMTs */
    CLONE_VMT(BaseClient, i_baseclient);
    CLONE_VMT(ClientMode, i_clientmode);
    CLONE_VMT(EngineVGui, i_enginevgui);

    dlclose(h_client);
    dlclose(h_engine);
    dlclose(h_matsurface);
    dlclose(h_sdl2);

    /* Individual functions/globals from signatures */
    if (!get_sigs())
        return false;

    /* Initialize global cache */
    cache_reset();
    cache_update();
    if (g.IsInGame)
        g.localidx = METHOD(i_engine, GetLocalPlayer);

    return true;
}

bool resore_vtables(void) {
    RESTORE_VMT(BaseClient, i_baseclient);
    RESTORE_VMT(ClientMode, i_clientmode);
    RESTORE_VMT(EngineVGui, i_enginevgui);

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
        .tall   = 13,
        .weight = 700,
        .flags  = FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS,
        .id     = METHOD(i_surface, CreateFont),
    };

    /* Create fonts with the data */
    CREATE_FONT(g_fonts.main);
    CREATE_FONT(g_fonts.small);
}

/*----------------------------------------------------------------------------*/

void cache_reset(void) {
    g.IsInGame    = false;
    g.IsConnected = false;
    g.MaxClients  = 0;
    g.MaxEntities = 0;

    for (int i = 0; i < (int)LENGTH(g.ents); i++)
        g.ents[i] = NULL;
}

void cache_update(void) {
    g.IsInGame    = METHOD(i_engine, IsInGame);
    g.IsConnected = METHOD(i_engine, IsConnected);

    if (g.IsInGame) {
        g.localplayer = METHOD_ARGS(i_entitylist, GetClientEntity, g.localidx);
        if (g.localplayer)
            g.IsAlive = METHOD(g.localplayer, IsAlive);

        g.MaxClients  = METHOD(i_engine, GetMaxClients);
        g.MaxEntities = METHOD(i_entitylist, GetMaxEntities);

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

            /* TODO: Only store the kinds of entities that we use */
            g.ents[i] = ent;
        }
    }
}
