
#include "features.h"
#include "../include/sdk.h"
#include "../include/globals.h"

static float old_frametime = 0.f;
static float old_curtime   = 0.f;

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

    /* TODO: Game movement, move helper, prediction interface */
}

void pred_end(void) {
    if (!g.localplayer)
        return;

    /* TODO: Finish game movement prediction, set move helper host to NULL */

    /* If cmd parameter is null, function sets m_nPredictionRandomSeed to -1 */
    SetPredictionRandomSeed(NULL);

    c_globalvars->frametime = old_frametime;
    c_globalvars->curtime   = old_curtime;
}
