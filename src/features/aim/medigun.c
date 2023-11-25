
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

static vec3_t get_best_center(vec3_t local_shoot_pos) {
    /* These vars are used to store the best target across iterations */
    vec3_t best_center          = VEC_ZERO;
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
            VEC_COPY(best_center, ent_center);
        }
    }

    return best_center;
}

void automedigun(usercmd_t* cmd) {
    if (!settings.automedigun || !g.localplayer || !g.localweapon || !g.IsAlive)
        return;

    if (METHOD(g.localweapon, GetWeaponId) != TF_WEAPON_MEDIGUN)
        return;

    /*--------------------------------------------------------------------*/

    /* Ticks since last target switch */
    static int last_release = 0;

    /* TODO: Setting */
    const int switch_time = 30;

    /* A tick has passed since last release */
    last_release++;

    if (!can_shoot() || last_release >= switch_time) {
        /* Every `switch_time` seconds, release attack for 1 tick */
        cmd->buttons &= ~IN_ATTACK;

        /* Store we just did that */
        last_release = 0;
        return;
    }

    /* If we are already healing, hold it */
    if (IsMedigunHealing(g.localweapon)) {
        cmd->buttons |= IN_ATTACK;
        return;
    }

    /*--------------------------------------------------------------------*/

    vec3_t local_shoot_pos = METHOD(g.localplayer, GetShootPos);

	/* Get center of best teammate, depending on health percentage */
    vec3_t target_pos = get_best_center(local_shoot_pos);

    /* No valid target */
    if (vec_is_zero(target_pos))
        return;

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
