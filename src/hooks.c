
#include <stdbool.h>
#include <stdio.h>
#include "include/hooks.h"
#include "include/globals.h"
#include "include/sdk.h"
#include "include/menu.h"
#include "features/features.h"

DECL_HOOK(CreateMove);
DECL_HOOK(Paint);

SwapWindow_t ho_SwapWindow = NULL;
PollEvent_t ho_PollEvent   = NULL;

/*----------------------------------------------------------------------------*/

bool hooks_init(void) {
    HOOK(i_clientmode->vmt, CreateMove);
    HOOK(i_enginevgui->vmt, Paint);

    HOOK_SDL(SwapWindow);
    HOOK_SDL(PollEvent);

    return true;
}

bool hooks_restore(void) {
    /* We don't worry about VMT hooks since we restore the whole VMT.
     * Restore SDL pointers */
    *SwapWindowPtr = ho_SwapWindow;
    *PollEventPtr  = ho_PollEvent;

    return true;
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

    if (settings.slide_walk)
        cmd->buttons ^= IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT;

    return ret;
}

/*----------------------------------------------------------------------------*/

void h_Paint(EngineVGui* thisptr, uint32_t mode) {
    ORIGINAL(Paint, thisptr, mode);

    if (mode & PAINT_UIPANELS) {
        StartDrawing(i_surface);

        player_esp();

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

    /* Render the menu */
    if (menu_open) {
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
