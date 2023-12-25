
#include "features.h"
#include "../include/sdk.h"
#include "../include/math.h"
#include "../include/globals.h"
#include "../include/settings.h"

static inline bool is_user_shooting(usercmd_t* cmd) {
    const bool using_melee = METHOD(g.localweapon, GetSlot) == WPN_SLOT_MELEE;
    if (using_melee)
        return melee_dealing_damage(cmd);

    if (!can_shoot())
        return false;

    static bool was_holding_attack = false;
    const bool is_holding_attack   = cmd->buttons & IN_ATTACK;

    const int weapon_id = METHOD(g.localweapon, GetWeaponId);
    if (weapon_id == TF_WEAPON_SNIPERRIFLE_CLASSIC ||
        weapon_id == TF_WEAPON_PIPEBOMBLAUNCHER) {
        const bool ret     = (was_holding_attack && !is_holding_attack);
        was_holding_attack = is_holding_attack;
        return ret;
    } else {
        return is_holding_attack;
    }
}

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
    if (is_user_shooting(cmd))
        return;

    /* Make static so spinbot angles persist over calls */
    static float spin_yaw = 0.f;
    float final_yaw       = cmd->viewangles.y;

    if (settings.aa_spin && settings.aa_speed > 0.f) {
        /* Spinbot */
        spin_yaw += settings.aa_speed / 4;
        if (spin_yaw > 180.f)
            spin_yaw -= 360.f;

        final_yaw = spin_yaw;
    } else {
        /* Add fixed amount to current angles */
        final_yaw += settings.aa_yaw;

        /* Reset spin yaw */
        spin_yaw = cmd->viewangles.y;
    }

    /* Desync */
    if (!bSendPacket)
        final_yaw += 58.f;

    /* Set fixed pitch and yaw we just calculated */
    cmd->viewangles.x = settings.aa_pitch;
    cmd->viewangles.y = final_yaw;

    ang_clamp(&cmd->viewangles);

    /* Micromovement for updating the LBY */
    if (fabsf(cmd->sidemove) < 5.0f) {
        if (cmd->buttons & IN_DUCK)
            /* We are ducking, make the movement bigger */
            cmd->sidemove = (cmd->tick_count & 1) ? 3.25f : -3.25f;
        else
            /* Otherwise, make it normal */
            cmd->sidemove = (cmd->tick_count & 1) ? 1.1f : -1.1f;
    }
}
