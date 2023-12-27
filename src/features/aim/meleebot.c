
#include <limits.h>
#include <time.h>
#include "../features.h"
#include "../../include/sdk.h"
#include "../../include/math.h"
#include "../../include/globals.h"
#include "../../include/settings.h"

/* Set in h_SwapWindow */
bool meleebot_key_down = false;

/*----------------------------------------------------------------------------*/
/* Melee crithack */

#define CRIT_TICKS_TO_PRED 60

static int get_next_crit_tick(usercmd_t* cmd) {
    int cmd_num = cmd->command_number;

    /* While we don't have a crit tick stored, check the next N ticks */
    for (int i = 0; i < CRIT_TICKS_TO_PRED; i++) {
        cmd->random_seed = MD5_PseudoRandom(cmd_num) & INT_MAX;
        SetPredictionRandomSeed(cmd);

        /* Store crit tick for when we are attacking */
        if (METHOD(g.localweapon, CalcIsAttackCriticalHelper))
            break;

        cmd_num++;
    }

    /* Reset seed to the original command_number */
    cmd->random_seed = MD5_PseudoRandom(cmd->command_number) & INT_MAX;
    SetPredictionRandomSeed(cmd);

    return cmd_num;
}

/* FIXME: This is getting called more than once when attacking, and it's causing
 * network lag */
static void melee_crithack(usercmd_t* cmd) {
    if (!settings.crits_melee || !g.localplayer || !g.localweapon || !g.IsAlive)
        return;

    /* Make sure we are currently using melee */
    if (METHOD(g.localweapon, GetSlot) != WPN_SLOT_MELEE)
        return;

#if 0
    /* NOTE: The caller is already checking this, uncomment if moved out of
     * meleebot */

    /* Get tick when we are attacking */
    if (!(cmd->buttons & IN_ATTACK) || !can_shoot())
        return;
#endif

    /* Set seed for rand() */
    static bool seed_changed = false;
    if (!seed_changed) {
        srand(time(NULL));
        seed_changed = true;
    }

    /* Check against user crit change */
    if ((rand() % 100) >= settings.crits_chance)
        return;

    /* Set the current cmd number to the next crit tick */
    const int crit_tick = get_next_crit_tick(cmd);
    cmd->command_number = crit_tick;
    cmd->random_seed    = MD5_PseudoRandom(crit_tick) & INT_MAX;
}

/*----------------------------------------------------------------------------*/

static inline bool attack_key(usercmd_t* cmd) {
    /* If keycode is 0, we use mouse1 as key */
    if (settings.melee_on_key && settings.melee_keycode != 0)
        return meleebot_key_down;
    else
        return cmd->buttons & IN_ATTACK;
}

static bool in_swing_range(vec3_t start, vec3_t end, Entity* target) {
    static vec3_t swing_mins = { -18.0f, -18.0f, -18.0f };
    static vec3_t swing_maxs = { 18.0f, 18.0f, 18.0f };

    Trace_t trace;
    TraceHull(start, end, swing_mins, swing_maxs, MASK_SHOT, &trace);

    return trace.entity == target;
}

static vec3_t get_melee_angle(void) {
    const float swing_range = METHOD(g.localweapon, GetSwingRange);
    if (swing_range <= 0.f)
        return VEC_ZERO;

    vec3_t local_eyes = METHOD(g.localplayer, EyePosition);
    vec3_t shoot_pos  = METHOD(g.localplayer, GetShootPos);

    /* Start closest_dist as range*N to filter far enemies */
    float closest_dist  = swing_range * 10.f;
    vec3_t closest_pos  = VEC_ZERO;
    Entity* closest_ent = NULL;

    /* Store hitbox position of closest enemy */
    for (int i = 1; i <= g.MaxClients; i++) {
        Entity* ent = g.ents[i];

        if (!ent || IsTeammate(ent))
            continue;

        if (!settings.aim_target_invul && IsInvulnerable(ent))
            continue;

        if (!settings.aim_target_friends && IsSteamFriend(ent))
            continue;

        if (!settings.aim_target_invisible && IsInvisible(ent))
            continue;

        /* Use the center of the entity's collision box */
        vec3_t target_pos = GetCenter(ent);
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

        static const float delay = 0.18f;

        /* Calculate velocity difference between us and the target rather than
         * just our velocity because he is also moving. */
        vec3_t velocity_diff =
          vec_sub(g.localplayer->velocity, closest_ent->velocity);

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
        shoot_pos = vec_add(shoot_pos, vec_flmul(velocity_diff, delay));

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

    return enemy_angle;
}

/*----------------------------------------------------------------------------*/

void meleebot(usercmd_t* cmd) {
    if (!settings.meleebot || !g.localplayer || !g.localweapon ||
        !attack_key(cmd))
        return;

    const bool we_are_attacking = g.localweapon->smackTime != -1.f;

    /* We are not starting to attack and we are not mid-attack */
    if (!can_shoot() && !we_are_attacking)
        return;

    /* We are being spectated in 1st person and we want to hide it */
    if (settings.melee_off_spectated && g.spectated_1st) {
        cmd->buttons |= IN_ATTACK;
        return;
    }

    const int wpn_slot = METHOD(g.localweapon, GetSlot);
    if (wpn_slot != WPN_SLOT_MELEE)
        return;

    /* NOTE: For meleebot we use cosest distance instead of FOV. This function
     * will also check if its in swing range, and predict positions after the
     * swing. */
    vec3_t target_angle = get_melee_angle();

    if (vec_is_zero(target_angle)) {
        /* We didn't find a valid target, we want to auto-attack on key, and
         * the keycode is 0 (mouse1): Don't attack */
        if (settings.melee_on_key && settings.melee_keycode == 0)
            cmd->buttons &= ~IN_ATTACK;

        return;
    }

    /* If we are not in the middle of an attack, start one */
    if (!we_are_attacking && settings.melee_on_key) {
        cmd->buttons |= IN_ATTACK;

        /* NOTE: Run the melee crithack here since every attack seems too much.
         * This can be moved to the CreateMove hook directly (after prediction,
         * for example), but keep in mind that it will force crits on all melee
         * weapons like the spy knife, engineer wrench when repairing, etc. */
        melee_crithack(cmd);
    }

    /* If we are actually going to deal damage in this tick (attack animation is
     * over), look to the target */
    if (melee_dealing_damage(cmd)) {
        /* We have no smoothing for meleebot */
        cmd->viewangles = target_angle;
        g.psilent       = true;

        /* If we don't want silent meleebot, change engine viewangles too */
        if (!settings.melee_silent)
            METHOD_ARGS(i_engine, SetViewAngles, &cmd->viewangles);
    }
}
