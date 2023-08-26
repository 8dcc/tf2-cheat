
#include "features.h"
#include "../include/sdk.h"
#include "../include/globals.h"

static float old_frametime = 0.f;
static float old_curtime   = 0.f;
static CMoveData move_data;

void pred_start(usercmd_t* cmd) {
    if (!g.localplayer)
        return;

    /* Store for pred_end() */
    old_frametime = c_globalvars->frametime;
    old_curtime   = c_globalvars->curtime;

    cmd->random_seed = MD5_PseudoRandom(cmd->command_number) & 0x7FFFFFFF;
    SetPredictionRandomSeed(cmd);

    c_globalvars->frametime = c_globalvars->interval_per_tick;
    c_globalvars->curtime =
      p.localplayer->nTickBase * c_globalvars->interval_per_tick;

    memset(&move_data, 0, sizeof(CMoveData));
    /* TODO: MoveHelper->SetHost */
    /* TODO: Prediction->SetupMove */
    METHOD(i_gamemovement, ProcessMovement, g.localplayer, &move_data);
    /* TODO: Prediction->FinishMove */
}

void pred_end(void) {
    if (!g.localplayer)
        return;

    METHOD(i_gamemovement, FinishTrackPredictionErrors, g.localplayer);
    /* TODO: MoveHelper->SetHost(NULL) */

    /* If cmd parameter is null, function sets m_nPredictionRandomSeed to -1 */
    SetPredictionRandomSeed(NULL);

    c_globalvars->frametime = old_frametime;
    c_globalvars->curtime   = old_curtime;
}
