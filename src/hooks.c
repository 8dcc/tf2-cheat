
#include <stdbool.h>
#include <stdio.h>
#include "include/hooks.h"
#include "include/globals.h"
#include "include/sdk.h"
#include "include/menu.h"
#include "features/features.h"

DECL_HOOK(LevelShutdown);
DECL_HOOK(LevelInitPostEntity);
DECL_HOOK(FrameStageNotify);
DECL_HOOK(CreateMove);
DECL_HOOK(Paint);

SwapWindow_t ho_SwapWindow = NULL;
PollEvent_t ho_PollEvent   = NULL;

/*----------------------------------------------------------------------------*/

bool hooks_init(void) {
    HOOK(i_baseclient->vmt, LevelShutdown);
    HOOK(i_baseclient->vmt, LevelInitPostEntity);
    HOOK(i_baseclient->vmt, FrameStageNotify);
    HOOK(i_clientmode->vmt, CreateMove);
    HOOK(i_enginevgui->vmt, Paint);

    HOOK_SDL(SwapWindow);
    HOOK_SDL(PollEvent);

    return true;
}

bool hooks_restore(void) {
    /* We don't worry about VMT hooks since we restore the whole VMT.
     * Here we restore SDL pointers */
    *SwapWindowPtr = ho_SwapWindow;
    *PollEventPtr  = ho_PollEvent;

    return true;
}

/*----------------------------------------------------------------------------*/

void h_LevelShutdown(BaseClient* thisptr) {
    ORIGINAL(LevelShutdown, thisptr);

    /* Reset all on LevelShutdown */
    g.localplayer = -1;
    cache_reset();
}

void h_LevelInitPostEntity(BaseClient* thisptr) {
    ORIGINAL(LevelInitPostEntity, thisptr);

    /* Get once on LevelInit */
    g.localplayer = METHOD(i_engine, GetLocalPlayer);
}

void h_FrameStageNotify(BaseClient* thisptr, ClientFrameStage_t curStage) {
    ORIGINAL(FrameStageNotify, thisptr, curStage);

    switch (curStage) {
        case FRAME_NET_UPDATE_START:
            /* Reset because we don't want to cache a removed pointer */
            cache_reset();
            break;
        case FRAME_NET_UPDATE_END:
            /* Cache information for the rest of the cheat */
            cache_update();
            break;
        default:
            break;
    }
}

/*----------------------------------------------------------------------------*/

bool h_CreateMove(ClientMode* thisptr, float flInputSampleTime,
                  usercmd_t* cmd) {
    bool ret = ORIGINAL(CreateMove, thisptr, flInputSampleTime, cmd);

    localplayer = METHOD_ARGS(i_entitylist, GetClientEntity,
                              METHOD(i_engine, GetLocalPlayer));
    if (!localplayer)
        return ret;

    bhop(cmd);
    autobackstab(cmd);

    if (settings.slide_walk)
        cmd->buttons ^= IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT;

    return ret;
}

/*----------------------------------------------------------------------------*/

void h_Paint(EngineVGui* thisptr, uint32_t mode) {
    ORIGINAL(Paint, thisptr, mode);

    if (mode & PAINT_UIPANELS) {
        StartDrawing(i_surface);

        esp();

        FinishDrawing(i_surface);
    }
}

/*----------------------------------------------------------------------------*/

void h_SwapWindow(SDL_Window* window) {
    /* Initialize once */
    if (!ctx)
        if (!menu_init(window))
            return ORIGINAL(SwapWindow, window);

    /* Switch to our gl context */
    SDL_GL_MakeCurrent(window, gl_ctx);

    /* Toggle menu and cursor */
    if (nk_input_is_key_released(&ctx->input, MENU_KEY)) {
        menu_open = !menu_open;
        METHOD_ARGS(i_surface, SetCursorAlwaysVisible, menu_open);
    }

    /* Render the watermark and menu */
    if (settings.watermark || menu_open) {
        if (settings.watermark)
            watermark_render();

        if (settings.speclist)
            spectator_list();

        if (menu_open)
            menu_render();

        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY,
                      MAX_ELEMENT_MEMORY);
    }

    /* Switch to original gl context */
    SDL_GL_MakeCurrent(window, orig_gl_ctx);

    /* Call original */
    nk_input_begin(ctx);
    ORIGINAL(SwapWindow, window);
    nk_input_end(ctx);
}

int h_PollEvent(SDL_Event* event) {
    int ret = ORIGINAL(PollEvent, event);

    /*
     * Credits:
     * https://github.com/deboogerxyz/ah4/blob/93e135135716e4d840547eea3c1ad922714ef9b7/hooks.c#L65
     */
    if (ret && nk_sdl_handle_event(event) && menu_open)
        event->type = 0;

    return ret;
}
