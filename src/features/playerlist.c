#include "features.h"
#include "../include/playerlist.h"
#include "../include/sdk.h"
#include "../include/globals.h"
#include <stdio.h>

void playerlist_full_update() {
    for (int i = 0; i < MAXPLAYERS; i++) {
        const int ent_i = i + 1;
        Entity* ent     = g.ents[ent_i];
        if (ent_i > g.MaxClients || !ent) {
            g.playerlist_players[i].is_good = false;
            continue;
        }

        const bool is_a_steam_friend = IsSteamFriend(ent);
        player_info_t player_info;
        METHOD_ARGS(i_engine, GetPlayerInfo, ent_i, &player_info);
        /* TODO: */
        bool should_be_ignored;
        enum PLAYER_LIST_PLAYER_PRESET preset;
        /* Check if already had a player on same index and its still the same
         * person */
        player_list_player_t* old_playerlist_player = &g.playerlist_players[i];
        if (old_playerlist_player->is_good &&
            player_info.userID == old_playerlist_player->player_info.userID) {
            should_be_ignored = old_playerlist_player->should_be_ignored;
            preset            = old_playerlist_player->preset;
        } else {
            should_be_ignored = false;
            preset            = UNSET;
        }

        g.playerlist_players[i] =
          (player_list_player_t){ is_a_steam_friend, player_info,
                                  should_be_ignored, preset, true };
    }
}

void playerlist_print_all() {
    for (int i = 0; i < MAXPLAYERS; i++) {
        player_list_player_t* playerlist_player = &g.playerlist_players[i];
        /* Check if player is valid before printing him */
        if (!playerlist_player || !playerlist_player->is_good ||
            !playerlist_player->player_info.ishltv) {
            continue;
        }
        printf("Player %d, name %s, userid: %d, preset: %d, target "
               "ignored: %s, is a bot: %s\n",
               i, playerlist_player->player_info.name,
               playerlist_player->player_info.userID, playerlist_player->preset,
               playerlist_player->should_be_ignored ? "true" : "false",
               playerlist_player->player_info.fakeplayer ? "true" : "false");
    }
}