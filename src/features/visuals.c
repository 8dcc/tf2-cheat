
#include "features.h"
#include "../include/sdk.h"
#include "../include/math.h"
#include "../include/globals.h"

/*----------------------------------------------------------------------------*/

#define SPEC_LINE_H 14

void spectator_list(void) {
    /* Reset global even if spec list is disabled */
    g.spectated_1st = false;

    if (!settings.speclist || !g.localplayer || !g.IsInGame)
        return;

    if (settings.clean_screenshots && METHOD(i_engine, IsTakingScreenshot))
        return;

    /* If we are dead, display spectator list for the guy we are spectating */
    Entity* local = g.IsAlive ? g.localplayer
                              : METHOD(g.localplayer, GetObserverTarget);
    if (!local)
        return;

    int spec_x = 0, spec_y = 0;
    METHOD_ARGS(i_engine, GetScreenSize, &spec_x, &spec_y);

    spec_x = 5;
    spec_y = (float)spec_y * (settings.speclist_height / 100.f);

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

static vec3_t localvelocity    = VEC_ZERO;
static float localvelocity_len = -1.f;
static float jump_velocity = 0.f, old_jump_velocity = 0.f;

static inline rgba_t speed2col(float speed) {
    if (speed > 900.f)
        speed = 900.f;
    const rgba_t converted = hue2rgba(speed / 1024.f * 280.f);
    return col_scale(converted, 1.35);
}

void store_velocity(void) {
    if (!g.localplayer || !g.IsInGame)
        return;

    Entity* local = g.IsAlive ? g.localplayer
                              : METHOD(g.localplayer, GetObserverTarget);
    if (!local || !METHOD(local, IsAlive)) {
        /* Mark as invalid */
        localvelocity_len = -1.f;
        return;
    }

    localvelocity     = local->velocity;
    localvelocity_len = vec_len2d(localvelocity);

    static bool was_on_ground = false;
    const bool is_on_ground   = (g.localplayer->flags & FL_ONGROUND) != 0;

    /* If we are too slow, reset jump velocities. Otherwise, every tick we start
     * a jump, update them. */
    if (localvelocity_len <= 30.f) {
        old_jump_velocity = 0.f;
        jump_velocity     = 0.f;
    } else if (was_on_ground && !is_on_ground) {
        old_jump_velocity = jump_velocity;
        jump_velocity     = localvelocity_len;
    }

    was_on_ground = is_on_ground;
}

void draw_velocity(void) {
    if (!g.localplayer || !g.IsInGame)
        return;

    if (settings.clean_screenshots && METHOD(i_engine, IsTakingScreenshot))
        return;

    /* Marked as invalid by store_velocity() */
    if (localvelocity_len == -1.f)
        return;

    static const rgba_t white = (rgba_t){ 220, 220, 220, 255 };
    static char txt_speed[]   = "9999999999";
    const HFont font          = g_fonts.main.id;

    int screen_w, screen_h;
    METHOD_ARGS(i_engine, GetScreenSize, &screen_w, &screen_h);

    int text_x = screen_w / 2;
    int text_y = (float)screen_h * (settings.draw_velocity_pos / 100.f);

    /* X position when drawing "999". Used as base position for "S:" prefixes */
    int three_digits_x, three_digits_h;
    get_text_size(font, "999", &three_digits_x, &three_digits_h);
    three_digits_x = text_x - (three_digits_x / 2);

    if (settings.draw_velocity) {
        rgba_t speed_col = speed2col(localvelocity_len);

        /* Convert speed to string */
        sprintf(txt_speed, "%d", (int)localvelocity_len);

        int text_w, text_h;
        get_text_size(font, txt_speed, &text_w, &text_h);

        /* Draw current speed centered */
        text_x -= (text_w / 2);
        draw_text(text_x, text_y, false, font, speed_col, txt_speed);

        /* For "S: " prefix */
        int prefix_w, prefix_h;
        get_text_size(font, "S: ", &prefix_w, &prefix_h);

        /* If number is bigger than "999", adapt */
        int prefix_x = (text_x < three_digits_x) ? text_x - prefix_w
                                                 : three_digits_x - prefix_w;
        draw_text(prefix_x, text_y, false, font, white, "S: ");

        if (settings.draw_velocity_jump && jump_velocity > 0.f) {
            rgba_t speed_col = (jump_velocity > old_jump_velocity)
                                 ? (rgba_t){ 49, 235, 52, 255 }
                                 : (rgba_t){ 247, 57, 57, 255 };

            text_x += text_w;
            draw_text(text_x, text_y, false, font, white, " (");
            get_text_size(font, " (", &text_w, &text_h);
            text_x += text_w;

            sprintf(txt_speed, "%d", (int)jump_velocity);
            draw_text(text_x, text_y, false, font, speed_col, txt_speed);
            get_text_size(font, txt_speed, &text_w, &text_h);
            text_x += text_w;

            draw_text(text_x, text_y, false, font, white, ")");
        }

        text_y += VELOCITY_LINE_H;
    }

    if (settings.draw_velocity_vert) {
        const float vert_vel = ABS(localvelocity.z);
        rgba_t speed_col     = speed2col(vert_vel);

        text_x = screen_w / 2;

        sprintf(txt_speed, "%d", (int)vert_vel);

        int text_w, text_h;
        get_text_size(font, txt_speed, &text_w, &text_h);

        /* Draw current speed centered */
        text_x -= (text_w / 2);
        draw_text(text_x, text_y, false, g_fonts.main.id, speed_col, txt_speed);

        /* Draw "V: " prefix */
        int prefix_w, prefix_h;
        get_text_size(font, "V: ", &prefix_w, &prefix_h);

        /* If number is bigger than "999", adapt */
        int prefix_x = (text_x < three_digits_x) ? text_x - prefix_w
                                                 : three_digits_x - prefix_w;
        draw_text(prefix_x, text_y, false, font, white, "V: ");
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

    if (settings.clean_screenshots && METHOD(i_engine, IsTakingScreenshot))
        return;

    /* We don't want to overwrite when scoped, and we are scoped */
    if (!settings.custom_fov_scoped && InCond(g.localplayer, TF_COND_ZOOMED))
        return;

    pSetup->fov = settings.custom_fov_deg;
}
