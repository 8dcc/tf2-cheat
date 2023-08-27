
#include "features.h"
#include "../include/sdk.h"
#include "../include/globals.h"

static float old_curtime   = 0.f;
static float old_frametime = 0.f;
static int old_tickcount   = 0;
static CMoveData move_data;

/* NOTE: Prediction is a bit of a black box for me. Feel free to make a GitHub
 * issue with more information. */
void pred_start(usercmd_t* cmd) {
    if (!g.localplayer)
        return;

    /* Store for pred_end() */
    old_curtime   = c_globalvars->curtime;
    old_frametime = c_globalvars->frametime;
    old_tickcount = c_globalvars->tickcount;

    cmd->random_seed = MD5_PseudoRandom(cmd->command_number) & 0x7FFFFFFF;
    SetPredictionRandomSeed(cmd);

    const int tick_base = p.localplayer->nTickBase;

    c_globalvars->curtime   = tick_base * c_globalvars->interval_per_tick;
    c_globalvars->frametime = c_globalvars->interval_per_tick;
    c_globalvars->tickcount = tick_base;

    memset(&move_data, 0, sizeof(CMoveData));
    /* TODO: Prediction->SetupMove */
    METHOD(i_gamemovement, ProcessMovement, g.localplayer, &move_data);
    /* TODO: Prediction->FinishMove */
}

void pred_end(void) {
    if (!g.localplayer)
        return;

    METHOD(i_gamemovement, FinishTrackPredictionErrors, g.localplayer);

    /* If cmd parameter is null, function sets m_nPredictionRandomSeed to -1 */
    SetPredictionRandomSeed(NULL);

    c_globalvars->curtime   = old_curtime;
    c_globalvars->frametime = old_frametime;
    c_globalvars->tickcount = old_tickcount;
}
