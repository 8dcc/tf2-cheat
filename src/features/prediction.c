
#include <limits.h>
#include "features.h"
#include "../include/sdk.h"
#include "../include/globals.h"

static float old_curtime            = 0.f;
static float old_frametime          = 0.f;
static int old_tickcount            = 0;
static int old_flags                = 0;
static bool old_bInPrediction       = false;
static bool old_bFirstTimePredicted = false;
static CMoveData movedata;

void pred_start(usercmd_t* cmd) {
    if (!g.localplayer)
        return;

    /* Store for pred_end() */
    old_curtime             = c_globalvars->curtime;
    old_frametime           = c_globalvars->frametime;
    old_tickcount           = c_globalvars->tickcount;
    old_flags               = g.localplayer->flags;
    old_bInPrediction       = i_prediction->m_bInPrediction;
    old_bFirstTimePredicted = i_prediction->m_bFirstTimePredicted;

    /* Get and set random seed */
    cmd->random_seed = MD5_PseudoRandom(cmd->command_number) & INT_MAX;
    SetPredictionRandomSeed(cmd);

    g.localplayer->m_pCurrentCommand = cmd;

    /* Update global vars */
    const int tick_base     = g.localplayer->nTickBase;
    c_globalvars->curtime   = tick_base * c_globalvars->interval_per_tick;
    c_globalvars->frametime = c_globalvars->interval_per_tick;
    c_globalvars->tickcount = tick_base;

    i_prediction->m_bInPrediction       = true;
    i_prediction->m_bFirstTimePredicted = false;

    /* Updated in h_RunCommand() */
    if (!i_movehelper)
        return;

    METHOD_ARGS(i_gamemovement, StartTrackPredictionErrors, g.localplayer);

    METHOD_ARGS(i_prediction, SetLocalViewAngles, &cmd->viewangles);

    memset(&movedata, 0, sizeof(CMoveData));
    METHOD_ARGS(i_prediction, SetupMove, g.localplayer, cmd, i_movehelper,
                &movedata);
    METHOD_ARGS(i_gamemovement, ProcessMovement, g.localplayer, &movedata);
    METHOD_ARGS(i_prediction, FinishMove, g.localplayer, cmd, &movedata);
}

void pred_end(void) {
    if (!g.localplayer)
        return;

    METHOD_ARGS(i_gamemovement, FinishTrackPredictionErrors, g.localplayer);

    /* If cmd parameter is null, function sets m_nPredictionRandomSeed to -1 */
    SetPredictionRandomSeed(NULL);

    g.localplayer->m_pCurrentCommand = NULL;

    c_globalvars->curtime               = old_curtime;
    c_globalvars->frametime             = old_frametime;
    c_globalvars->tickcount             = old_tickcount;
    g.localplayer->flags                = old_flags;
    i_prediction->m_bInPrediction       = old_bInPrediction;
    i_prediction->m_bFirstTimePredicted = old_bFirstTimePredicted;
}
