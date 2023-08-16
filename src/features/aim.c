
#include <math.h>
#include "features.h"
#include "../include/sdk.h"
#include "../include/globals.h"
#include "../include/settings.h"

static bool is_visible(vec3_t start, vec3_t end) {
    /* TODO: Trace ray from start to end, check if visible, etc.
     * TODO: Add ignore visible checkbox? */
    (void)start;
    (void)end;

    return true;
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

static vec3_t get_closest_delta(vec3_t viewangles) {
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

        /* TODO: Aim friendly checkbox? */
        if (!ent || IsTeammate(ent))
            continue;

        vec3_t target_head = get_hitbox_pos(ent, 0);
        if (vec_is_zero(target_head))
            continue;

        if (!is_visible(local_eyes, target_head)) /* We can't see player */
            continue;

        const vec3_t enemy_angle = vec_to_ang(vec_sub(target_head, local_eyes));
        vec3_t delta             = vec_sub(enemy_angle, viewangles);
        vec_norm(&delta);
        vec_clamp(&delta);

        float fov = hypotf(delta.x, delta.y);
        if (fov < best_fov) {
            best_fov = fov;
            VEC_COPY(best_delta, delta);
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

    /* Calculate delta with the engine viewangles, not with the cmd ones */
    vec3_t engine_viewangles;
    METHOD_ARGS(i_engine, GetViewAngles, &engine_viewangles);

    /* TODO: Add setting for lowest health instead of closest */
    vec3_t best_delta = get_closest_delta(engine_viewangles);

    if (!vec_is_zero(best_delta)) {
        const float aim_smooth =
          (settings.aim_smooth >= 1.f) ? settings.aim_smooth : 1.f;

        cmd->viewangles.x = engine_viewangles.x + best_delta.x / aim_smooth;
        cmd->viewangles.y = engine_viewangles.y + best_delta.y / aim_smooth;
        cmd->viewangles.z = engine_viewangles.z + best_delta.z / aim_smooth;
    }

    if (!settings.aim_silent)
        METHOD_ARGS(i_engine, SetViewAngles, &cmd->viewangles);
}
