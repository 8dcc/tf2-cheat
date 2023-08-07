
#include "features.h"
#include "../include/globals.h"
#include "../include/util.h"
#include "../include/sdk.h"

void autostrafe_legit(usercmd_t* cmd) {
    /* Check mouse x delta */
    if (cmd->mousedx < 0)
        cmd->sidemove = -450.0f;
    else if (cmd->mousedx > 0)
        cmd->sidemove = 450.0f;
}

/* TODO: Bhop and autostrafe menu/settings
 * TODO: Make autostrafe functions static after menu */
void bhop(usercmd_t* cmd) {
    if (!localplayer || !METHOD(localplayer, IsAlive))
        return;

    const bool is_jumping = (cmd->buttons & IN_JUMP) != 0;

    if (!(localplayer->flags & FL_ONGROUND))
        cmd->buttons &= ~IN_JUMP;

    if (is_jumping)
        autostrafe_legit(cmd);
}
