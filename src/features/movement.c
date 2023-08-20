
#include <stdio.h>
#include <math.h>
#include "features.h"
#include "../include/globals.h"
#include "../include/util.h"
#include "../include/sdk.h"

static void autostrafe_legit(usercmd_t* cmd) {
    /* Check mouse x delta */
    if (cmd->mousedx < 0)
        cmd->sidemove = -450.0f;
    else if (cmd->mousedx > 0)
        cmd->sidemove = 450.0f;
}

static void autostrafe_rage(usercmd_t* cmd) {
    /* TODO: Get at runtime */
    static const float sv_airaccelerate = 10.0f;
    static const float sv_maxspeed      = 320.0f;
    static const float cl_forwardspeed  = 450.0f;
    static const float cl_sidespeed     = 450.0f;

    const vec3_t velocity = g.localplayer->velocity;
    const float speed     = vec_len2d(velocity);

    /* If low speed, start forward */
    if (speed < 30 && (cmd->buttons & IN_FORWARD)) {
        cmd->forwardmove = 450.0f;
        return;
    }

    float term = sv_airaccelerate / sv_maxspeed * 100.0f / speed;
    if (term < -1 || term > 1)
        return;

    float best_delta = acosf(term);

    /* Use engine viewangles in case we do something nasty with cmd's angles */
    vec3_t viewangles;
    METHOD_ARGS(i_engine, GetViewAngles, &viewangles);

    /* Get our desired angles and delta */
    float yaw        = DEG2RAD(viewangles.y);
    float vel_dir    = atan2f(velocity.y, velocity.x) - yaw;
    float target_ang = atan2f(-cmd->sidemove, cmd->forwardmove);
    float delta      = angle_delta_rad(vel_dir, target_ang);

    float movedir = delta < 0 ? vel_dir + best_delta : vel_dir - best_delta;

    cmd->forwardmove = cosf(movedir) * cl_forwardspeed;
    cmd->sidemove    = -sinf(movedir) * cl_sidespeed;
}

void bhop(usercmd_t* cmd) {
    if (!settings.bhop || !g.IsAlive || !g.localplayer)
        return;

    const bool is_jumping = (cmd->buttons & IN_JUMP) != 0;

    if (!(g.localplayer->flags & FL_ONGROUND))
        cmd->buttons &= ~IN_JUMP;

    if (is_jumping) {
        switch (settings.autostrafe) {
            default:
            case SETT_OFF:
                break;
            case SETT_LEGIT:
                autostrafe_legit(cmd);
                break;
            case SETT_RAGE:
                autostrafe_rage(cmd);
                break;
        }
    }
}

void autorocketjump(usercmd_t* cmd) {
    if (!settings.rocketjump || !g.localplayer || !(cmd->buttons & IN_JUMP))
        return;

    /* If bhop is off, only rocketjump on ground */
    if (!settings.bhop && !(g.localplayer->flags & FL_ONGROUND))
        return;

    Weapon* weapon = METHOD(g.localplayer, GetWeapon);
    if (!weapon)
        return;

    Networkable* net       = GetNetworkable((Entity*)weapon);
    ClientClass* ent_class = METHOD(net, GetClientClass);
    if (!ent_class)
        return;

    /* Valid weapon */
    if (ent_class->class_id != CClass_CTFRocketLauncher &&
        ent_class->class_id != CClass_CTFRocketLauncher_AirStrike &&
        ent_class->class_id != CClass_CTFRocketLauncher_DirectHit &&
        ent_class->class_id != CClass_CTFRocketLauncher_Mortar)
        return;

    if (!can_shoot(g.localplayer))
        return;

    vec3_t viewangles;
    METHOD_ARGS(i_engine, GetViewAngles, &viewangles);

    /* Only rocketjump if we are looking at the floor */
    if (viewangles.x < 45.f)
        return;

    cmd->viewangles.x = 89.f;
    cmd->buttons |= IN_ATTACK | IN_DUCK | IN_JUMP;
    cmd->buttons &= ~IN_ATTACK2;
}

void correct_movement(usercmd_t* cmd, vec3_t old_angles) {
    float old_y = old_angles.y + (old_angles.y < 0 ? 360 : 0);
    float new_y = cmd->viewangles.y + (cmd->viewangles.y < 0 ? 360 : 0);
    float delta = (new_y < old_y) ? fabsf(new_y - old_y)
                                  : 360 - fabsf(new_y - old_y);

    delta = 360 - delta;

    float forward = cmd->forwardmove;
    float side    = cmd->sidemove;

    cmd->forwardmove =
      cos(DEG2RAD(delta)) * forward + cos(DEG2RAD(delta + 90)) * side;
    cmd->sidemove =
      sin(DEG2RAD(delta)) * forward + sin(DEG2RAD(delta + 90)) * side;
}
