#ifndef PLAYERLIST_H_
#define PLAYERLIST_H_

#include "sdk.h"

enum PLAYER_LIST_PLAYER_PRESET {
    UNSET  = 0,
    FRIEND = 1,
    /* Votekickers */
    SOFT_RAGE = 2,
    RAGE      = 3,
    /* Cheaters */
    MAX_RAGE = 4
};

typedef struct {
    /* Updates every tick */
    bool is_a_steam_friend;
    player_info_t player_info;
    /* Can be set through playerlist menu */
    bool should_be_ignored;
    enum PLAYER_LIST_PLAYER_PRESET preset;

    /* Check this before processing this struct */
    bool is_good;
} player_list_player_t;

#endif /* PLAYERLIST_H_ */