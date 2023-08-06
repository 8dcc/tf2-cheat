
#include <stdbool.h>
#include <stdio.h>
#include "include/hooks.h"
#include "include/globals.h"
#include "include/sdk.h"

DECL_HOOK(CreateMove);

/*----------------------------------------------------------------------------*/

bool hooks_init(void) {
    HOOK(i_clientmode->vt, CreateMove);

    return true;
}

/*----------------------------------------------------------------------------*/

bool h_CreateMove(ClientMode* thisptr, float flInputSampleTime,
                  usercmd_t* cmd) {
    bool ret = ORIGINAL(CreateMove, thisptr, flInputSampleTime, cmd);

    printf("Hello from CreateMove!\n");

    return ret;
}
