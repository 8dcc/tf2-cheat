
#include <stdbool.h>
#include <stdio.h>
#include "include/hooks.h"
#include "include/globals.h"
#include "include/sdk.h"
#include "features/features.h"

DECL_HOOK(CreateMove);

/*----------------------------------------------------------------------------*/

bool hooks_init(void) {
    HOOK(i_clientmode->vmt, CreateMove);

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
