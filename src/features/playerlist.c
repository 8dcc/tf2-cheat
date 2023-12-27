#include "features.h"
#include "../include/playerlist.h"
#include "../include/sdk.h"
#include "../include/globals.h"
#include <stdio.h>

void playerlist_print_all(usercmd_t* cmd) {
    for (int i = 0; i < g.MaxClients; i++) {
        player_list_player_t* playerlist_player = &g.playerlist_players[i];
        /* Check if player is valid before printing him */
        if (!playerlist_player || !playerlist_player->is_good ||
            playerlist_player->player_info.ishltv) {
            continue;
        }
        printf("PlayerList BEGIN: (TICK %d)", cmd->tick_count);
        printf("Player %d, name %s, userid: %d, preset: %d, target "
               "ignored: %s, is a bot: %s\n",
               i, playerlist_player->player_info.name,
               playerlist_player->player_info.userID, playerlist_player->preset,
               playerlist_player->should_be_ignored ? "true" : "false",
               playerlist_player->player_info.fakeplayer ? "true" : "false");
        printf("PlayerList END: (TICK %d)", cmd->tick_count);
    }
}