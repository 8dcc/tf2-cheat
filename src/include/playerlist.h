#ifndef PLAYERLIST_H_
#define PLAYERLIST_H_

#include "sdk.h"

enum EPlayerListPlayerPreset {
    UNSET     = 0,
    FRIEND    = 1,
    SOFT_RAGE = 2,
    RAGE      = 3,
};

typedef struct {
    /* Updated every tick */
    player_info_t pinfo;
    int is_steam_friend;
    /* TODO: is_party_member */

    /* Can be set through playerlist menu */
    int preset;
    int is_ignored;

    /* Should be true before processing the struct */
    int is_valid;
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
    }
}

static inline bool plist_is_friend(plist_player_t* player) {
    return player->is_steam_friend || player->preset == FRIEND;
}

static inline bool plist_is_rage(plist_player_t* player) {
    return player->preset == SOFT_RAGE || player->preset == RAGE;
}

void plist_print_all(void);

#endif /* PLAYERLIST_H_ */
