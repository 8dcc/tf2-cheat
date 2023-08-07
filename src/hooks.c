
#include <stdbool.h>
#include <stdio.h>
#include "include/hooks.h"
#include "include/globals.h"
#include "include/sdk.h"
#include "features/features.h"

DECL_HOOK(CreateMove);

SwapWindow_t ho_SwapWindow = NULL;
PollEvent_t ho_PollEvent   = NULL;

/*----------------------------------------------------------------------------*/

bool hooks_init(void) {
    HOOK(i_clientmode->vmt, CreateMove);

    HOOK_SDL(SwapWindow);
    HOOK_SDL(PollEvent);

    return true;
}

/*----------------------------------------------------------------------------*/

bool h_CreateMove(ClientMode* thisptr, float flInputSampleTime,
                  usercmd_t* cmd) {
    bool ret = ORIGINAL(CreateMove, thisptr, flInputSampleTime, cmd);

    localplayer = METHOD_ARGS(i_entitylist, GetClientEntity, 1);
    if (!localplayer)
        return ret;

    bhop(cmd);

    return ret;
}

/*----------------------------------------------------------------------------*/

void h_SwapWindow(SDL_Window* window) {
    printf("Hello from SwapWindow!\n");

    ORIGINAL(SwapWindow, window);
}

int h_PollEvent(SDL_Event* event) {
    int ret = ORIGINAL(PollEvent, event);

    printf("Hello from PollEvent!\n");

    return ret;
}
