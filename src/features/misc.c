
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

#define SPEC_LINE_H 14

void spectator_list(void) {
    /* Reset global even if spec list is disabled */
    g.spectated_1st = false;

    if (!settings.speclist || !g.localplayer || !g.IsInGame)
        return;

    /* If we are dead, display spectator list for the guy we are spectating */
    Entity* local = g.IsAlive ? g.localplayer
                              : METHOD(g.localplayer, GetObserverTarget);
    if (!local)
        return;

    int spec_x = 0, spec_y = 0;
    METHOD_ARGS(i_engine, GetScreenSize, &spec_x, &spec_y);

    spec_y = spec_y / 2 + 10;
    spec_x = 5;

    bool first_spectator = true;

    int maxclients = MIN(64, g.MaxClients);
    for (int i = 1; i <= maxclients; i++) {
        if (i == g.localidx)
            continue;

        Entity* ent      = METHOD_ARGS(i_entitylist, GetClientEntity, i);
        Networkable* net = GetNetworkable(ent);
        if (!ent || ent == g.localplayer || ent == local ||
            METHOD(net, IsDormant) || METHOD(ent, IsAlive))
            continue;

        /* Not spectating us */
        const int obs_mode = METHOD(ent, GetObserverMode);
        if (obs_mode == OBS_MODE_NONE ||
            METHOD(ent, GetObserverTarget) != local)
            continue;

        if (first_spectator) {
            draw_text(spec_x, spec_y, false, g_fonts.main.id,
                      (rgba_t){ 65, 186, 70, 255 }, "Spectators:");

            spec_y += SPEC_LINE_H + 1;
            first_spectator = false;
        }

        /* Add name to list */
        player_info_t pinfo;
        if (!METHOD_ARGS(i_engine, GetPlayerInfo, i, &pinfo))
            continue;

        static char converted[sizeof("[Freeze] ") + MAX_PLAYER_NAME_LENGTH];

        /* Actual location of the player name inside converted[] */
        char* name_pos = converted;

        switch (obs_mode) {
            default:
            case OBS_MODE_IN_EYE:
                strcpy(converted, "[1st] ");
                name_pos += sizeof("[1st] ") - 1;

                /* Used by aimbot if settings.aim_off_spectated is enabled */
                g.spectated_1st = true;
                break;
            case OBS_MODE_CHASE:
                strcpy(converted, "[3rd] ");
                name_pos += sizeof("[3rd] ") - 1;
                break;
            case OBS_MODE_ROAMING:
                strcpy(converted, "[Free] ");
                name_pos += sizeof("[Free] ") - 1;
                break;
            case OBS_MODE_FREEZECAM:
                strcpy(converted, "[Freeze] ");
                name_pos += sizeof("[Freeze] ") - 1;
                break;
            case OBS_MODE_DEATHCAM:
                strcpy(converted, "[Death] ");
                name_pos += sizeof("[Death] ") - 1;
                break;
            case OBS_MODE_FIXED:
                strcpy(converted, "[Fixed] ");
                name_pos += sizeof("[Fixed] ") - 1;
                break;
            case OBS_MODE_POI: /* Point of interest (game objective, etc.) */
                strcpy(converted, "[Poi] ");
                name_pos += sizeof("[Poi] ") - 1;
                break;
        }

        /* Replace multibyte chars with '?' */
        convert_player_name(name_pos, pinfo.name);

        draw_text(spec_x, spec_y, false, g_fonts.main.id,
                  (rgba_t){ 200, 200, 200, 255 }, converted);

        spec_y += SPEC_LINE_H;
    }
}

/*----------------------------------------------------------------------------*/

#define VELOCITY_LINE_H 14

static inline rgba_t speed2col(float speed) {
    if (speed > 900.f)
        speed = 900.f;
    const rgba_t converted = hue2rgba(speed / 1024.f * 280.f);
    return col_scale(converted, 1.2);
}

void draw_velocity(void) {
    if (!g.localplayer || !g.IsInGame || !g.IsAlive)
        return;

    static char txt_speed[] = "9999999999";

    int screen_w, screen_h;
    METHOD_ARGS(i_engine, GetScreenSize, &screen_w, &screen_h);

    int text_x = screen_w / 2;
    int text_y = (float)screen_h * (settings.draw_velocity_pos / 100.f);

    /* TODO: Draw "V: " before text (numx - numw/2 - txtw) */

    if (settings.draw_velocity) {
        const float cur_speed = vec_len2d(g.localvelocity);
        rgba_t speed_col      = speed2col(cur_speed);

        sprintf(txt_speed, "%d", (int)cur_speed);
        draw_text(text_x, text_y, true, g_fonts.main.id, speed_col, txt_speed);

        text_y += VELOCITY_LINE_H;
    }

    /* TODO: Draw jump speeds and color for improvements */

    if (settings.draw_velocity_vert) {
        const float cur_speed = ABS(g.localvelocity.z);
        rgba_t speed_col      = speed2col(cur_speed);

        sprintf(txt_speed, "%d", (int)cur_speed);
        draw_text(text_x, text_y, true, g_fonts.main.id, speed_col, txt_speed);
    }
}

/*----------------------------------------------------------------------------*/

void thirdperson(void) {
    static bool was_thirdperson = true;

    if (!g.localplayer)
        return;

    /* Thirdperson only for localplayer since I just play casual and I can
     * change view when spectating :) */
    if (!settings.thirdperson || !g.IsAlive) {
        if (was_thirdperson) {
            g.localplayer->nForceTauntCam = false;
            was_thirdperson               = false;
        }

        return;
    }

    g.localplayer->nForceTauntCam = true;
    was_thirdperson               = true;
}

/*----------------------------------------------------------------------------*/

void custom_fov(ViewSetup* pSetup) {
    if (!settings.custom_fov || !g.localplayer || !g.IsInGame || !g.IsAlive)
        return;

    /* We don't want to overwrite when scoped, ad we are scoped */
    if (!settings.custom_fov_scoped && InCond(g.localplayer, TF_COND_ZOOMED))
        return;

    pSetup->fov = settings.custom_fov_deg;
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
