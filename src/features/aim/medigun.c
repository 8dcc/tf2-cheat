
#include "../features.h"
#include "../../include/sdk.h"
#include "../../include/globals.h"

#define MEDIGUN_RANGE 449.0f

/* If we are looking X degrees away from the desired angle, start healing */
#define AIM_DEGREE_THRESHOD 3.f

/* Different from the one in aimbot.c */
static bool is_visible(vec3_t start, vec3_t end, Entity* target) {
    TraceFilter filter;
    TraceFilterInit(&filter, g.localplayer);

    Ray_t ray;
    RayInit(&ray, start, end);

    Trace_t trace;
    METHOD_ARGS(i_enginetrace, TraceRay, &ray, MASK_SHOT | CONTENTS_GRATE,
                &filter, &trace);

    return trace.entity == target || trace.fraction > 0.97f;
}

static Entity* get_best_target(vec3_t local_shoot_pos) {
    /* These vars are used to store the best target across iterations */
    Entity* best_ent            = NULL;
    float lowest_health_percent = 1.f;

    for (int i = 1; i <= g.MaxClients; i++) {
        Entity* ent = g.ents[i];

        if (!ent || !IsTeammate(ent) || METHOD(ent, GetIndex) == g.localidx)
            continue;

        /* Entity center from collision box */
        vec3_t ent_center = GetCenter(ent);
        if (vec_is_zero(ent_center))
            continue;

        float cur_dist = vec_dist(ent_center, local_shoot_pos);

        /* Too far, ignore */
        if (cur_dist > MEDIGUN_RANGE)
            continue;

        const float health_mult =
          (g.localweapon->m_iItemDefinitionIndex == Medic_s_TheQuickFix)
            ? 1.24f
            : 1.44f;

        const float real_max_health = METHOD(ent, GetMaxHealth) * health_mult;
        const float health          = METHOD(ent, GetHealth);

        /* Already at max possible health, ignore */
        if (health >= real_max_health)
            continue;

        /* We can't heal him */
        if (!is_visible(local_shoot_pos, ent_center, ent))
            continue;

        /* Health percentage relative to the max health after healing */
        const float real_health_percent = health / real_max_health;

        /* This teammate has less health than the best one, store */
        if (lowest_health_percent > real_health_percent) {
            lowest_health_percent = real_health_percent;
            best_ent              = ent;
        }
    }

    return best_ent;
}

void automedigun(usercmd_t* cmd) {
    if (!settings.automedigun || !g.localplayer || !g.localweapon || !g.IsAlive)
        return;

    if (METHOD(g.localweapon, GetWeaponId) != TF_WEAPON_MEDIGUN)
        return;

    if (!can_shoot())
        return;

    /* Calculate only once */
    const vec3_t local_shoot_pos  = METHOD(g.localplayer, GetShootPos);
    const bool is_medigun_healing = IsMedigunHealing(g.localweapon);

    /* Get best teammate, depending on health percentage */
    Entity* best_target = get_best_target(local_shoot_pos);

    /* No valid target, or we still need to wait */
    if (!best_target) {
        /* Even if there is no good target, if we are healing, don't stop */
        if (is_medigun_healing)
            cmd->buttons |= IN_ATTACK;

        return;
    }

    /* Did we release attack on the last tick? */
    static bool just_released = false;

    /* Curtime when we last switched target */
    static float last_switch = 0.f;

    /* Did enough time pass since last switch? Variable for readability */
    const bool switch_time_passed =
      c_globalvars->curtime >= last_switch + settings.automedigun_switch_time;

    if (is_medigun_healing) {
        /* Did we release the mouse on the last tick? Then we should not be
         * healing anyone, if we are, release again and stop */
        if (just_released) {
            cmd->buttons &= ~IN_ATTACK;
            return;
        }

        /* Get index of currently healed player */
        CBaseHandle healed_handler = GetMedigunHealingHandler(g.localweapon);
        const int healed_idx       = CBaseHandle_GetEntryIndex(healed_handler);

        /* If it's already the best possible target, hold it. Otherwise, release
         * attack so we can find the best target on the next tick. */
        if (METHOD(best_target, GetIndex) == healed_idx) {
            cmd->buttons |= IN_ATTACK;
            return;
        }

        /* If we are healing someone that is not the best target, but we still
         * need to wait, stop */
        if (!switch_time_passed) {
            cmd->buttons |= IN_ATTACK;
            return;
        }

        cmd->buttons &= ~IN_ATTACK;

        /* Store when we just released the mouse */
        just_released = true;

        /* Always return if we are already healing */
        return;
    }

    just_released = false;

    /* Get the entity center from collision box */
    vec3_t target_pos = GetCenter(best_target);

    /* Get target angle */
    vec3_t target_angle = vec_to_ang(vec_sub(target_pos, local_shoot_pos));

    if (settings.automedigun_silent) {
        /* If pSilent, ignore smoothing. Just set the angles. */
        VEC_COPY(cmd->viewangles, target_angle);
        g.psilent = true;
    } else {
        /* Otherwise, get delta from engine viewangles */
        vec3_t viewangles;
        METHOD_ARGS(i_engine, GetViewAngles, &viewangles);

        vec3_t delta = vec_sub(target_angle, viewangles);
        vec_norm(&delta);
        ang_clamp(&delta);

        /* Use smoothing, depending on user setting */
        const float aim_smooth = MAX(1.f, settings.automedigun_smooth);

        /* Change view, scaling with smoothing */
        cmd->viewangles.x = viewangles.x + delta.x / aim_smooth;
        cmd->viewangles.y = viewangles.y + delta.y / aim_smooth;
        cmd->viewangles.z = viewangles.z + delta.z / aim_smooth;

        /* Since we are not using silent, also change the engine viewangles */
        METHOD_ARGS(i_engine, SetViewAngles, &cmd->viewangles);
    }

    vec3_t new_delta = vec_sub(target_angle, cmd->viewangles);

    /* Are we looking at the target, or we need more ticks because of smoothing?
     * Only start healing when we are looking at him. */
    if (ABS(new_delta.x) < AIM_DEGREE_THRESHOD &&
        ABS(new_delta.y) < AIM_DEGREE_THRESHOD) {
        /* Store we just switched targets */
        last_switch = c_globalvars->curtime;

        /* Since we have it in front of us, attack */
        cmd->buttons |= IN_ATTACK;
    }
}
