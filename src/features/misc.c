
#include "features.h"
#include "../include/sdk.h"
#include "../include/math.h"
#include "../include/globals.h"

void autobackstab(usercmd_t* cmd) {
    if (!settings.autostab || !g.localplayer || !g.localweapon || !g.IsAlive)
        return;

    if (METHOD(g.localweapon, GetWeaponId) != TF_WEAPON_KNIFE)
        return;

    /* If setting, only check bReadyToBackstab, skip TraceHull */
    if (settings.anim_stab && g.localweapon->bReadyToBackstab) {
        cmd->buttons |= IN_ATTACK;
        return;
    }

    const float swing_range = METHOD(g.localweapon, GetSwingRange);
    if (swing_range <= 0.f)
        return;

    static const vec3_t swing_mins = { -18.0f, -18.0f, -18.0f };
    static const vec3_t swing_maxs = { 18.0f, 18.0f, 18.0f };

    const vec3_t shoot_pos    = METHOD(g.localplayer, GetShootPos);
    const vec3_t local_origin = *METHOD(g.localplayer, GetAbsOrigin);

    float closest_dist  = swing_range * 4.f;
    Entity* closest_ent = NULL;

    /* Store hitbox position of closest enemy */
    for (int i = 1; i <= g.MaxClients; i++) {
        Entity* ent                  = g.ents[i];
        plist_player_t* plist_player = &g.playerlist[i];
        if (!ent || !plist_player || !plist_player->is_valid)
            continue;

        if (!METHOD(ent, IsAlive) || IsTeammate(ent))
            continue;

        Networkable* net = GetNetworkable(ent);
        if (METHOD(net, IsDormant))
            continue;

        if (plist_player->is_ignored)
            continue;

        if (!settings.aim_target_invul && IsInvulnerable(ent))
            continue;

        if (!settings.aim_target_friends && plist_is_friend(plist_player))
            continue;

        if (!settings.aim_target_invisible && IsInvisible(ent))
            continue;

        vec3_t ent_origin = *METHOD(ent, GetAbsOrigin);
        if (vec_is_zero(ent_origin))
            continue;

        float dist = vec_len(vec_sub(local_origin, ent_origin));

        if (dist < closest_dist) {
            closest_dist = dist;
            closest_ent  = ent;
        }
    }

    if (!closest_ent)
        return;

    if (!IsBehindAndFacingTarget(g.localplayer, closest_ent))
        return;

    vec3_t forward   = ang_to_vec(cmd->viewangles);
    vec3_t swing_end = vec_add(shoot_pos, vec_flmul(forward, swing_range));

    Trace_t trace;
    TraceHull(shoot_pos, swing_end, swing_mins, swing_maxs, MASK_SHOT, &trace);

    if (trace.entity != closest_ent)
        return;

    /* Finally, attack */
    cmd->buttons |= IN_ATTACK;
}

/*----------------------------------------------------------------------------*/

void nopush(void) {
    static ConVar* pushaway = NULL;

    if (!pushaway) {
        pushaway = METHOD_ARGS(i_cvar, FindVar, "tf_avoidteammates_pushaway");
        return;
    }

    if (settings.nopush == ConVar_GetBool(pushaway))
        ConVar_SetInt(pushaway, !settings.nopush);
}

/*----------------------------------------------------------------------------*/

#define AFK_TIME 50.f /* seconds */

void antiafk(usercmd_t* cmd) {
    if (!settings.antiafk || !g.localplayer || !g.IsInGame)
        return;

    static float last_afk_time = 0;

    /* We are not AFK */
    if (cmd->buttons != 0) {
        last_afk_time = 0.f;
        return;
    }

    /* This is the first tick where buttons are zero, store time */
    if (last_afk_time == 0.f) {
        last_afk_time = c_globalvars->curtime;
        return;
    }

    static bool went_forward = false;

    /* If buttons have been zero for N seconds, move */
    if (c_globalvars->curtime - last_afk_time > AFK_TIME) {
        cmd->buttons |= (went_forward) ? IN_BACK : IN_FORWARD;
        went_forward  = !went_forward;
        last_afk_time = 0.f;
    }
}
