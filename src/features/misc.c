
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
        Entity* ent = g.ents[i];

        if (!ent || IsTeammate(ent))
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

void auto_detonate_stickies(usercmd_t* cmd) {
    if (!settings.auto_detonate || !g.localplayer || !g.localweapon ||
        !g.IsAlive)
        return;

    /* Invalid slot */
    const int wpn_slot = METHOD(g.localweapon, GetSlot);
    if (wpn_slot != WPN_SLOT_SECONDARY)
        return;

    /* TODO: Check if we have launcher in slot 2, instead of checking if it's
     * the current weapon. */
    /* Invalid secondary type */
    const int wpn_id = METHOD(g.localweapon, GetWeaponId);
    if (wpn_id != TF_WEAPON_PIPEBOMBLAUNCHER)
        return;

    /* Iterate entities, searching for stickies */
    for (int i = g.MaxClients + 1; i < g.MaxEntities; i++) {
        Entity* sticky = g.ents[i];
        if (!sticky)
            continue;

        Networkable* sticky_net   = GetNetworkable(sticky);
        ClientClass* sticky_class = METHOD(sticky_net, GetClientClass);
        if (!sticky_class)
            continue;

        /* CTFGrenadePipebombProjectile means both stickies and pipebombs */
        if (sticky_class->class_id != CClass_CTFGrenadePipebombProjectile)
            continue;

        /* Is it a sticky or a pipebomb? */
        if (!IsStickyBomb(sticky))
            continue;

        /* Are we the owner of the stickybomb? */
        CBaseHandle thrower_handle = GetThrowerHandle(sticky);
        const int thrower_idx      = CBaseHandle_GetEntryIndex(thrower_handle);
        if (thrower_idx != g.localidx)
            continue;

        /* TODO: Check if the sticky is ready to be detonated */

        /* Valid sticky. Calculate position once. */
        const vec3_t sticky_pos = *METHOD(sticky, WorldSpaceCenter);

        /* Current entity is one of our stickies, check if it's close enough to
         * an enemy. */
        for (int j = 1; j < g.MaxClients; j++) {
            Entity* player = g.ents[j];
            if (!player)
                continue;

            /* If we are the current player, just check if we want to detonate
             * ourselves. Otherwise, also make sure the player is a vulnerable
             * enemy. */
            if (j == g.localidx) {
                if (!settings.auto_detonate_self)
                    continue;
            } else if (IsTeammate(player) || IsInvulnerable(player)) {
                continue;
            }

            /* TODO: If scotish resistance, look to the sticky entity with
             * g.pSilent */

            /* Is it close enough? */
            const vec3_t player_pos = GetCenter(player);
            const float distance    = vec_len(vec_sub(sticky_pos, player_pos));

            if (distance > settings.auto_detonate_dist)
                continue;

            /* Is there anything between the sticky and the target? */
            if (is_enemy_visible(sticky_pos, player_pos, player)) {
                cmd->buttons |= IN_ATTACK2;
                return;
            }
        }
    }
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
