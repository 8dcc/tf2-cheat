
#include "features.h"
#include "../include/sdk.h"
#include "../include/math.h"
#include "../include/globals.h"
#include "../include/settings.h"

void spinbot(usercmd_t* cmd) {
    if (!settings.aa || !g.IsAlive || !g.localplayer)
        return;

    /* Wrong movetype, like noclip */
    if (GetMoveType(g.localplayer) != MOVETYPE_WALK)
        return;

    /* Make static so spinbot angles persist over calls */
    static float spin_yaw = 0.f;

    /* Fixed pitch */
    cmd->viewangles.x = settings.aa_pitch;

    if (settings.aa_spin) {
        /* Spinbot */
        spin_yaw += settings.aa_speed / 4;
        if (spin_yaw > 180.f)
            spin_yaw -= 360.f;
        cmd->viewangles.y = spin_yaw;
    } else {
        /* Add fixed amount to current angles */
        cmd->viewangles.y += settings.aa_yaw;
    }

    ang_clamp(&cmd->viewangles);
}
