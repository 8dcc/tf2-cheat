
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

    /* Don't enable in water */
    if (g.localplayer->flags & FL_INWATER || g.localplayer->flags & FL_SWIM ||
        g.localplayer->m_nWaterLevel > WL_Feet)
        return;

    /* User is manually shooting, disable for that tick */
    const bool using_melee = METHOD(g.localweapon, GetSlot) == WPN_SLOT_MELEE;
    if ((using_melee && melee_dealing_damage(cmd)) ||
        (!using_melee && can_shoot() && cmd->buttons & IN_ATTACK))
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

        /* Reset spin yaw */
        spin_yaw = cmd->viewangles.y;
    }

    ang_clamp(&cmd->viewangles);
}
