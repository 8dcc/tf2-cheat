
#include <stdio.h>
#include "include/playerlist.h"
#include "include/sdk.h"
#include "include/globals.h"

void plist_print_all(usercmd_t* cmd) {
    for (int i = 0; i < g.MaxClients; i++) {
        plist_player_t* player = &g.playerlist[i];

        /* Check if player is valid before printing him */
        if (!player || !player->is_good || player->pinfo.ishltv)
            continue;

        printf("PlayerList BEGIN: (TICK %d)\n", cmd->tick_count);
        printf(
          "[%d] Name %s | UserID: %d | Preset: %d | Ignored: %d | Is bot: %d\n",
          i, player->pinfo.name, player->pinfo.userID, player->preset,
          player->is_ignored, player->pinfo.fakeplayer);
        printf("PlayerList END: (TICK %d)\n\n", cmd->tick_count);
    }
}
