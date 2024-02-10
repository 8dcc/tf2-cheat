
#include <stdio.h>
#include <math.h>
#include "features.h"
#include "../include/globals.h"
#include "../include/sdk.h"
#include "../include/util.h"
#include "../include/math.h"

static void autostrafe_legit(usercmd_t* cmd) {
    /* Check mouse x delta */
    if (cmd->mousedx < 0)
        cmd->sidemove = -450.0f;
    else if (cmd->mousedx > 0)
        cmd->sidemove = 450.0f;
}

static void autostrafe_rage(usercmd_t* cmd) {
    const vec3_t velocity = g.localplayer->velocity;
    const float speed     = vec_len2d(velocity);

    /* If low speed, start forward */
    if (speed < 30)
        return;

    float term = g.sv_airaccelerate / g.sv_maxspeed * 100.0f / speed;
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

    cmd->forwardmove = cosf(movedir) * g.cl_forwardspeed;
    cmd->sidemove    = -sinf(movedir) * g.cl_sidespeed;
}

void bhop(usercmd_t* cmd) {
    if (!settings.bhop || !g.IsAlive || !g.localplayer)
        return;

    /* Wrong movetype, like noclip */
    if (GetMoveType(g.localplayer) != MOVETYPE_WALK)
        return;

    /* Don't do anything in water */
    if (g.localplayer->flags & FL_INWATER || g.localplayer->flags & FL_SWIM ||
        g.localplayer->m_nWaterLevel > WL_Feet)
        return;

    static bool was_jumping = false;
    const bool is_jumping   = (cmd->buttons & IN_JUMP) != 0;
    const bool is_on_ground = (g.localplayer->flags & FL_ONGROUND) != 0;

    /* NOTE: We need `was_jumping' for scout's double-jump */
    if (!is_on_ground && was_jumping) {
        cmd->buttons &= ~IN_JUMP;

        /* Only autostrafe when not double-jumping and when holding space
         * mid-air */
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

    was_jumping = is_jumping;
}

void autorocketjump(usercmd_t* cmd) {
    if (!settings.rocketjump || !g.localplayer || !g.localweapon ||
        !(cmd->buttons & IN_ATTACK2) || !(g.localplayer->flags & FL_ONGROUND))
        return;

    const int wpn_id = METHOD(g.localweapon, GetWeaponId);
    if (wpn_id != TF_WEAPON_ROCKETLAUNCHER &&
        wpn_id != TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT)
        return;

    if (!can_shoot())
        return;

    vec3_t velocity = g.localplayer->velocity;

    if (velocity.x == 0.f && velocity.y == 0.f) {
        /* If we are not moving, aim down and reverse yaw */
        cmd->viewangles.x = 89.f;

        /* For normal rocketlaunchers, reverting jaw directly is not ideal
         * because the rocket shoots from an offset, we use -85º for all except
         * "The Original" */
        if (g.localweapon->m_iItemDefinitionIndex == Soldier_m_TheOriginal)
            cmd->viewangles.y = sub_offset_to_yaw(cmd->viewangles.y, 180.f);
        else
            cmd->viewangles.y = sub_offset_to_yaw(cmd->viewangles.y, 85.f);
    } else {
        vec3_t velocity_ang = velocity_to_ang(velocity);

        /* Angle pitch will be the user angle (default 45º) and the yaw will be
         * reversed velocity angle */
        cmd->viewangles.x = settings.rocketjump_deg;
        cmd->viewangles.y = velocity_ang.y - 180.f;

        /* See previous comment when stopped. We need to add since offset is
         * already inverted */
        if (g.localweapon->m_iItemDefinitionIndex != Soldier_m_TheOriginal) {
            if (settings.rocketjump_deg >= 70.f)
                cmd->viewangles.y = add_offset_to_yaw(cmd->viewangles.y, 45.f);
            else if (settings.rocketjump_deg >= 80.f)
                cmd->viewangles.y = add_offset_to_yaw(cmd->viewangles.y, 75.f);
        }
    }

    /* Release hotkey and hold rocketjump keys. Make it pSilent too */
    cmd->buttons &= ~IN_ATTACK2;
    cmd->buttons |= IN_ATTACK | IN_DUCK | IN_JUMP;
    g.psilent = true;

    vec_norm(&cmd->viewangles);
    ang_clamp(&cmd->viewangles);
}

void correct_movement(usercmd_t* cmd, vec3_t old_angles) {
    /* Don't correct movement if we are taunting with antiaim disabled */
    if (!g.localplayer ||
        (!settings.aa && g.localplayer->m_bAllowMoveDuringTaunt))
        return;

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
