
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

/*----------------------------------------------------------------------------*/

void* h_client     = NULL;
void* h_engine     = NULL;
void* h_matsurface = NULL;
void* h_sdl2       = NULL;

Entity* localplayer = NULL;

SwapWindow_t* SwapWindowPtr = NULL;
PollEvent_t* PollEventPtr   = NULL;

DECL_INTF(BaseClient, baseclient);
DECL_INTF(EngineClient, engine);
DECL_INTF(EntityList, entitylist);
DECL_INTF(EngineVGui, enginevgui);
DECL_INTF(MatSurface, surface);
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

    /* Other interfaces */
    i_clientmode = get_clientmode();
    if (!i_clientmode || !i_clientmode->vmt) {
        fprintf(stderr, "globals_init: couldn't load i_clientmodebms\n");
        return false;
    }

    CLONE_VMT(ClientMode, i_clientmode);
    CLONE_VMT(EngineVGui, i_enginevgui);

    dlclose(h_client);
    dlclose(h_engine);
    dlclose(h_matsurface);
    dlclose(h_sdl2);

    return true;
}

bool resore_vtables(void) {
    RESTORE_VMT(ClientMode, i_clientmode);
    RESTORE_VMT(EngineVGui, i_enginevgui);

    return true;
}
