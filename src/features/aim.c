
#include <math.h>
#include "features.h"
#include "../include/sdk.h"
#include "../include/globals.h"
#include "../include/settings.h"

/*----------------------------------------------------------------------------*/
/* Common */

enum weapon_type {
    AIM   = 0,
    MELEE = 1,
};

static bool valid_weapon(enum weapon_type type) {
    Weapon* weapon = METHOD(g.localplayer, GetWeapon);
    if (!weapon)
        return false;

    /* For now just check if the current weapon is in a valid slot.
     * TODO: Add projectile aimbot depending on weapon->GetDamageType */
    int slot = METHOD(weapon, GetSlot);
    return (type == AIM)
             ? (slot == WPN_SLOT_PRIMARY || slot == WPN_SLOT_SECONDARY)
             : (slot == WPN_SLOT_MELEE);
}

static inline void setting_to_hitboxes(int setting, int* min, int* max) {
    switch (setting) {
        case SETT_HITBOX_HEAD:
            *min = HITBOX_HEAD;
            *max = HITBOX_HEAD;
            break;
        case SETT_HITBOX_TORSO:
            *min = HITBOX_PELVIS;
            *max = HITBOX_SPINE3;
            break;
        case SETT_HITBOX_ARMS:
            *min = HITBOX_LEFT_UPPER_ARM;
            *max = HITBOX_RIGHT_HAND;
            break;
        case SETT_HITBOX_LEGS:
            *min = HITBOX_LEFT_HIP;
            *max = HITBOX_RIGHT_FOOT;
            break;
    }
}

#define HITBOX_SET 0
static vec3_t get_hitbox_pos(Entity* ent, int hitbox_idx) {
    static matrix3x4_t bones[MAXSTUDIOBONES];

    Renderable* rend = GetRenderable(ent);

    if (!METHOD_ARGS(rend, SetupBones, bones, MAXSTUDIOBONES,
                     BONE_USED_BY_HITBOX, 0))
        return VEC_ZERO;

    const model_t* model = METHOD(rend, GetModel);
    if (!model)
        return VEC_ZERO;

    studiohdr_t* hdr = METHOD_ARGS(i_modelinfo, GetStudioModel, model);
    if (!hdr)
        return VEC_ZERO;

    return center_of_hitbox(hdr, bones, HITBOX_SET, hitbox_idx);
}

/*----------------------------------------------------------------------------*/
/* Aimbot */

static bool is_visible(vec3_t start, vec3_t end, Entity* target) {
    if (settings.aim_ignore_visible)
        return true;

    /* We initialize with a custom ShouldHitEntity() for ignoring teammates */
    TraceFilter filter;
    TraceFilterInit_IgnoreFriendly(&filter, g.localplayer);

    Ray_t ray;
    RayInit(&ray, start, end);

    Trace_t trace;
    METHOD_ARGS(i_enginetrace, TraceRay, &ray, MASK_SHOT | CONTENTS_GRATE,
                &filter, &trace);

    return trace.entity == target || trace.fraction > 0.97f;
}

static vec3_t get_closest_fov_delta(vec3_t viewangles) {
    /* Compensate aim punch */
    viewangles.x += g.localplayer->vecPunchAngle.x;
    viewangles.y += g.localplayer->vecPunchAngle.y;
    viewangles.z += g.localplayer->vecPunchAngle.z;

    vec3_t local_eyes = METHOD(g.localplayer, EyePosition);

    /* These 2 vars are used to store the best target across iterations.
     * NOTE: The initial value of best_fov will be the aimbot fov */
    float best_fov    = settings.aim_fov;
    vec3_t best_delta = { 0, 0, 0 };

    for (int i = 1; i <= g.MaxClients; i++) {
        Entity* ent = g.ents[i];

        if (!ent || IsTeammate(ent))
            continue;

        int minhitbox = 0, maxhitbox = 0;
        setting_to_hitboxes(settings.aim_hitbox, &minhitbox, &maxhitbox);

        for (int j = minhitbox; j <= maxhitbox; j++) {
            vec3_t target_pos = get_hitbox_pos(ent, j);
            if (vec_is_zero(target_pos))
                continue;

            /* We can't see current hitbox */
            if (!is_visible(local_eyes, target_pos, ent))
                continue;

            const vec3_t enemy_angle =
              vec_to_ang(vec_sub(target_pos, local_eyes));

            vec3_t delta = vec_sub(enemy_angle, viewangles);
            vec_norm(&delta);
            vec_clamp(&delta);

            float fov = hypotf(delta.x, delta.y);
            if (fov < best_fov) {
                best_fov = fov;
                VEC_COPY(best_delta, delta);
            }
        }
    }

    return best_delta;
}

void aimbot(usercmd_t* cmd) {
    if (!settings.aimbot || !(cmd->buttons & IN_ATTACK) || !g.localplayer ||
        !can_shoot(g.localplayer))
        return;

    /* We are being spectated in 1st person and we want to hide it */
    if (settings.aim_off_spectated && g.spectated_1st)
        return;

    if (!valid_weapon(AIM))
        return;

    /* Calculate delta with the engine viewangles, not with the cmd ones */
    vec3_t engine_viewangles;
    METHOD_ARGS(i_engine, GetViewAngles, &engine_viewangles);

    /* TODO: Add setting for lowest health instead of closest */
    vec3_t best_delta = get_closest_fov_delta(engine_viewangles);

    if (!vec_is_zero(best_delta)) {
        const float aim_smooth =
          (settings.aim_smooth >= 1.f) ? settings.aim_smooth : 1.f;

        cmd->viewangles.x = engine_viewangles.x + best_delta.x / aim_smooth;
        cmd->viewangles.y = engine_viewangles.y + best_delta.y / aim_smooth;
        cmd->viewangles.z = engine_viewangles.z + best_delta.z / aim_smooth;

        if (settings.aim_silent)
            *bSendPacket = false;
    } else if (settings.aim_shoot_if_target) {
        cmd->buttons &= ~IN_ATTACK;
    }

    if (!settings.aim_silent)
        METHOD_ARGS(i_engine, SetViewAngles, &cmd->viewangles);
}

/*----------------------------------------------------------------------------*/
/* Aimbot FOV circle */

static inline float scale_fov_by_width(float fov, float aspect_ratio) {
    aspect_ratio *= 0.75f;

    float half_angle_rad = fov * (0.5f * M_PI / 180.f);
    float t              = tan(half_angle_rad) * aspect_ratio;
    float retDegrees     = (180.f / M_PI) * atan(t);

    return retDegrees * 2.0f;
}

void draw_aim_fov(void) {
    if (!settings.aimbot || !settings.aim_draw_fov || !g.localplayer ||
        !g.IsAlive || !valid_weapon(AIM))
        return;

    /* Circle won't fit on the screen */
    if (settings.aim_fov > 90.0f)
        return;

    int sw = 0, sh = 0;
    METHOD_ARGS(i_engine, GetScreenSize, &sw, &sh);

    /* Get localplayer fov from ViewSetup */
    static ViewSetup playerview;
    if (!METHOD_ARGS(i_baseclient, GetPlayerView, &playerview))
        return;

    const float unscaled_fov = playerview.fov;

    /* Calculate radius */
    const float aspect_ratio = (float)sw / (float)sh;
    const float screen_fov   = scale_fov_by_width(unscaled_fov, aspect_ratio);
    const float x1           = tan(DEG2RAD(settings.aim_fov));
    const float x2           = tan(DEG2RAD(screen_fov) / 2.f);
    const float rad          = (x1 / x2) * (sw / 2);

    const rgba_t col = NK2COL(settings.col_aim_fov);
    METHOD_ARGS(i_surface, SetColor, col.r, col.g, col.b, col.a);
    METHOD_ARGS(i_surface, DrawCircle, sw / 2, sh / 2, rad, 255);
}

/*----------------------------------------------------------------------------*/
/* Melee bot */

static bool in_swing_range(vec3_t start, vec3_t end, Entity* target) {
    static vec3_t swing_mins = { -18.0f, -18.0f, -18.0f };
    static vec3_t swing_maxs = { 18.0f, 18.0f, 18.0f };

    Trace_t trace;
    TraceHull(start, end, swing_mins, swing_maxs, MASK_SHOT, &trace);

    return trace.entity == target;
}

static vec3_t get_melee_delta(vec3_t viewangles) {
    Weapon* weapon = METHOD(g.localplayer, GetWeapon);
    if (!weapon)
        return VEC_ZERO;

    const float swing_range = METHOD(weapon, GetSwingRange);
    if (swing_range <= 0.f)
        return VEC_ZERO;

    vec3_t shoot_pos  = METHOD(g.localplayer, EyePosition);
    vec3_t local_eyes = METHOD(g.localplayer, GetShootPos);

    /* Start closest_dist as range*4 to filter far enemies */
    float closest_dist  = swing_range * 4.f;
    vec3_t closest_pos  = { 0, 0, 0 };
    Entity* closest_ent = NULL;

    /* Store hitbox position of closest enemy */
    for (int i = 1; i <= g.MaxClients; i++) {
        Entity* ent = g.ents[i];

        if (!ent || IsTeammate(ent))
            continue;

        /* Use head if we are on air, torso otherwise */
        vec3_t target_pos = (g.localplayer->flags & FL_ONGROUND)
                              ? get_hitbox_pos(ent, HITBOX_SPINE1)
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
    const vec3_t swing_end =
      vec_add(shoot_pos, vec_flmul(ang_to_vec(enemy_angle), swing_range));

    /* We can't hit the current player */
    if (!in_swing_range(shoot_pos, swing_end, closest_ent))
        return VEC_ZERO;

    vec3_t delta = vec_sub(enemy_angle, viewangles);
    vec_norm(&delta);
    vec_clamp(&delta);

    return delta;
}

void meleebot(usercmd_t* cmd) {
    if (!settings.meleebot || !(cmd->buttons & IN_ATTACK) || !g.localplayer ||
        !can_shoot(g.localplayer))
        return;

    /* We are being spectated in 1st person and we want to hide it */
    if (settings.melee_off_spectated && g.spectated_1st)
        return;

    if (!valid_weapon(MELEE))
        return;

    vec3_t engine_viewangles;
    METHOD_ARGS(i_engine, GetViewAngles, &engine_viewangles);

    /* NOTE: For meleebot we use cosest distance instead of FOV. This function
     * will also check if its in swing range */
    vec3_t best_delta = get_melee_delta(engine_viewangles);

    if (!vec_is_zero(best_delta)) {
        /* No smoothing for meleebot */
        cmd->viewangles.x = engine_viewangles.x + best_delta.x;
        cmd->viewangles.y = engine_viewangles.y + best_delta.y;
        cmd->viewangles.z = engine_viewangles.z + best_delta.z;

        if (settings.melee_silent)
            *bSendPacket = false;
    } else if (settings.melee_shoot_if_target) {
        cmd->buttons &= ~IN_ATTACK;
    }

    /* TODO: pSilent */
    if (!settings.melee_silent)
        METHOD_ARGS(i_engine, SetViewAngles, &cmd->viewangles);
}
