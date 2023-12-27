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

#define PLAYER_LIST_PRESET_CCHAR_UNSET     "Unset";
#define PLAYER_LIST_PRESET_CCHAR_FRIEND    "Friend";
#define PLAYER_LIST_PRESET_CCHAR_SOFT_RAGE "Soft Rage";
#define PLAYER_LIST_PRESET_CCHAR_RAGE      "Rage";
#define PLAYER_LIST_PRESET_CCHAR_MAX_RAGE  "Max Rage";

const char* player_list_preset_to_const_char(
  const enum PLAYER_LIST_PLAYER_PRESET preset);

typedef struct {
    /* Updates every tick */
    bool is_a_steam_friend;
    // TODO: is_a_party_member
    player_info_t player_info;
    Entity* entity;
    /* Can be set through playerlist menu */
    bool should_be_ignored;
    enum PLAYER_LIST_PLAYER_PRESET preset;
    /* Check this before processing this struct */
    bool is_good;
} player_list_player_t;

#endif /* PLAYERLIST_H_ */