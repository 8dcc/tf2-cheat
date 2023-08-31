
#include "../features.h"
#include "../../include/sdk.h"
#include "../../include/globals.h"
#include "../../include/settings.h"
#include "common.h"

/* Set in h_SwapWindow */
bool meleebot_key_down = false;

static bool melee_attacking(usercmd_t* cmd) {
    if (METHOD(g.localweapon, GetWeaponId) == TF_WEAPON_KNIFE)
        return (cmd->buttons & IN_ATTACK) && can_shoot();

    /* TODO: Temporary until I add prediction */
    const float flTime =
      g.localplayer->nTickBase * c_globalvars->interval_per_tick;

    /* Credits: SEOwned (and afaik to KGB as well) */
    return fabs(g.localweapon->smackTime - flTime) <
           c_globalvars->interval_per_tick * 2.0f;
}

static bool in_swing_range(vec3_t start, vec3_t end, Entity* target) {
    static vec3_t swing_mins = { -18.0f, -18.0f, -18.0f };
    static vec3_t swing_maxs = { 18.0f, 18.0f, 18.0f };

    Trace_t trace;
    TraceHull(start, end, swing_mins, swing_maxs, MASK_SHOT, &trace);

    return trace.entity == target;
}

static vec3_t get_melee_delta(vec3_t viewangles) {
    const float swing_range = METHOD(g.localweapon, GetSwingRange);
    if (swing_range <= 0.f)
        return VEC_ZERO;

    vec3_t shoot_pos  = METHOD(g.localplayer, EyePosition);
    vec3_t local_eyes = METHOD(g.localplayer, GetShootPos);

    /* Start closest_dist as range*4 to filter far enemies */
    float closest_dist  = swing_range * 8.f;
    vec3_t closest_pos  = { 0, 0, 0 };
    Entity* closest_ent = NULL;

    /* Store hitbox position of closest enemy */
    for (int i = 1; i <= g.MaxClients; i++) {
        Entity* ent = g.ents[i];

        if (!ent || IsTeammate(ent))
            continue;

        /* Use head if we are on air, torso otherwise */
        vec3_t target_pos = (g.localplayer->flags & FL_ONGROUND)
                              ? get_hitbox_pos(ent, HITBOX_SPINE3)
                              : get_hitbox_pos(ent, HITBOX_HEAD);
        if (vec_is_zero(target_pos))
            continue;

        float dist = vec_len(vec_sub(shoot_pos, target_pos));

        if (dist < closest_dist) {
            closest_dist = dist;
            VEC_COPY(closest_pos, target_pos);
            closest_ent = ent;
        }
    }

    if (!closest_ent)
        return VEC_ZERO;

    const vec3_t enemy_angle = vec_to_ang(vec_sub(closest_pos, local_eyes));
    const vec3_t forward     = ang_to_vec(enemy_angle);
    vec3_t swing_end = vec_add(shoot_pos, vec_flmul(forward, swing_range));

    /* We can't hit the current player */
    if (!in_swing_range(shoot_pos, swing_end, closest_ent)) {
        if (!settings.melee_swing_pred ||
            METHOD(g.localweapon, GetWeaponId) == TF_WEAPON_KNIFE)
            return VEC_ZERO;

        static const float delay = 0.2f;

        /* Extrapolate position for getting new shootpos.
         * If we know we travel 5 units in a second (velocity), we can just
         * multiply that by the time we want to extrapolate to get the units
         * traveled in N seconds. Then we can just add that to the current
         * position.
         *
         * See also:
         *   - SEOwned
         *   - https://casualhacks.net/blog/2019-09-17/projectile-solver/
         *   - https://en.wikipedia.org/wiki/Extrapolation
         *   - https://en.wikipedia.org/wiki/Interpolation
         */
        shoot_pos =
          vec_add(shoot_pos, vec_flmul(g.localplayer->velocity, delay));

        if (!(g.localplayer->flags & FL_ONGROUND)) {
            /* TODO: Get from cvar at runtime */
            const float sv_gravity = 800.f;
            shoot_pos.z -= sv_gravity * 0.5f * delay * delay;
        }

        /* Calculate end of trace again and check if we can hit */
        swing_end = vec_add(shoot_pos, vec_flmul(forward, swing_range));

        if (!in_swing_range(shoot_pos, swing_end, closest_ent))
            return VEC_ZERO;
    }

    vec3_t delta = vec_sub(enemy_angle, viewangles);
    vec_norm(&delta);
    ang_clamp(&delta);

    return delta;
}

/*----------------------------------------------------------------------------*/

void meleebot(usercmd_t* cmd) {
    if (!settings.meleebot || !g.localplayer || !g.localweapon)
        return;

    if ((!settings.melee_on_key && !(cmd->buttons & IN_ATTACK)) ||
        (settings.melee_on_key && !meleebot_key_down))
        return;

    /* We are not starting to attack and we are not mid-attack */
    if (!can_shoot() && g.localweapon->smackTime == -1.f)
        return;

    /* We are being spectated in 1st person and we want to hide it */
    if (settings.melee_off_spectated && g.spectated_1st)
        return;

    const int wpn_slot = METHOD(g.localweapon, GetSlot);
    if (wpn_slot != WPN_SLOT_MELEE)
        return;

    vec3_t engine_viewangles;
    METHOD_ARGS(i_engine, GetViewAngles, &engine_viewangles);

    /* NOTE: For meleebot we use cosest distance instead of FOV. This function
     * will also check if its in swing range */
    vec3_t best_delta = get_melee_delta(engine_viewangles);

    if (!vec_is_zero(best_delta)) {
        /* Only move camera when we deal damage, not when we start attacking */
        if (melee_attacking(cmd)) {
            /* No smoothing for meleebot */
            cmd->viewangles.x = engine_viewangles.x + best_delta.x;
            cmd->viewangles.y = engine_viewangles.y + best_delta.y;
            cmd->viewangles.z = engine_viewangles.z + best_delta.z;

            if (settings.melee_silent)
                *bSendPacket = false;
        }

        if (settings.melee_on_key)
            cmd->buttons |= IN_ATTACK;
    }

    if (!settings.melee_silent)
        METHOD_ARGS(i_engine, SetViewAngles, &cmd->viewangles);
}
