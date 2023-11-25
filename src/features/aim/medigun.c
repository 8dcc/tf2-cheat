
#include "../features.h"
#include "../../include/sdk.h"
#include "../../include/globals.h"

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
        if (cur_dist > 449.0f)
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

    /* We can't shoot, stop */
    if (!can_shoot()) {
        cmd->buttons &= ~IN_ATTACK;
        return;
    }

    vec3_t local_shoot_pos = METHOD(g.localplayer, GetShootPos);

    /* Get best teammate, depending on health percentage */
    Entity* best_target = get_best_target(local_shoot_pos);

    /* No valid target */
    if (!best_target) {
        /* Even if there is no good target, if we are healing, don't stop */
        if (IsMedigunHealing(g.localweapon))
            cmd->buttons |= IN_ATTACK;

        return;
    }

    /* Ticks since we switched target */
    static uint32_t ticks_since_switch = 0;

    /* Every tick, increase count */
    ticks_since_switch++;

    /* TODO: Setting */
    const int switch_time = 30;

    /* If we are already healing, and enough ticks have passed since last switch
     * depending on user setting. */
    if (IsMedigunHealing(g.localweapon) && ticks_since_switch >= switch_time) {
        CBaseHandle healed_handler = GetMedigunHealingHandler(g.localweapon);
        const int healed_idx       = CBaseHandle_GetEntryIndex(healed_handler);
        if (healed_idx < 1 || healed_idx >= g.MaxClients)
            return;

        /* If it's already the best possible target, hold it. Otherwise, release
         * attack so we can find the best target on the next tick */
        if (METHOD(best_target, GetIndex) == healed_idx) {
            cmd->buttons |= IN_ATTACK;
        } else {
            cmd->buttons &= ~IN_ATTACK;

            /* Store we switched on this tick */
            ticks_since_switch = 0;
        }

        /* Always return if we are already healing */
        return;
    }

    /* Get the entity center from collision box */
    vec3_t target_pos = GetCenter(best_target);

    /* Get target angle */
    vec3_t target_angle = vec_to_ang(vec_sub(target_pos, local_shoot_pos));

    if (settings.automedigun_silent) {
        /* If pSilent, ignore smoothing. Just set the angles. */
        VEC_COPY(cmd->viewangles, target_angle);
        *bSendPacket = false;
    } else {
        /* Otherwise, get delta from engine viewangles */
        vec3_t viewangles;
        METHOD_ARGS(i_engine, GetViewAngles, &viewangles);

        vec3_t delta = vec_sub(target_angle, viewangles);
        vec_norm(&delta);
        ang_clamp(&delta);

        /* Use smoothing, depending on user setting */
        const float aim_smooth = MAX(settings.automedigun_smooth, 1.f);

        /* Change view, scaling with smoothing */
        cmd->viewangles.x = viewangles.x + delta.x / aim_smooth;
        cmd->viewangles.y = viewangles.y + delta.y / aim_smooth;
        cmd->viewangles.z = viewangles.z + delta.z / aim_smooth;

        /* Since we are not using silent, also change the engine viewangles */
        METHOD_ARGS(i_engine, SetViewAngles, &cmd->viewangles);
    }

    /* Finally, attack */
    cmd->buttons |= IN_ATTACK;
}
