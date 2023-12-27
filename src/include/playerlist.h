#ifndef PLAYERLIST_H_
#define PLAYERLIST_H_

#include "sdk.h"

enum EPlayerListPlayerPreset {
    UNSET  = 0,
    FRIEND = 1,
    /* Votekickers */
    SOFT_RAGE = 2,
    RAGE      = 3,
    /* Cheaters */
    MAX_RAGE = 4
};

typedef struct {
    /* Updated every tick */
    player_info_t pinfo;
    bool is_steam_friend;
    /* TODO: is_a_party_member */

    /* Can be set through playerlist menu */
    int preset;
    bool is_ignored;

    /* Should be true before processing the struct */
    bool is_good;
} plist_player_t;

/*----------------------------------------------------------------------------*/

static inline const char* plist_preset_name(enum EPlayerListPlayerPreset num) {
    switch (num) {
        default:
        case UNSET:
            return "Unset";
        case FRIEND:
            return "Friend";
        case SOFT_RAGE:
            return "Soft Rage";
        case RAGE:
            return "Rage";
        case MAX_RAGE:
            return "Max Rage";
    }
}

static inline bool plist_is_friend(plist_player_t* player) {
    return player->is_steam_friend || player->preset == FRIEND;
}

#endif /* PLAYERLIST_H_ */
