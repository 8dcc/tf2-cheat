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

#define PLIST_PRESET_STR_UNSET     "Unset"
#define PLIST_PRESET_STR_FRIEND    "Friend"
#define PLIST_PRESET_STR_SOFT_RAGE "Soft Rage"
#define PLIST_PRESET_STR_RAGE      "Rage"
#define PLIST_PRESET_STR_MAX_RAGE  "Max Rage"

const char* plist_preset_to_str(enum EPlayerListPlayerPreset preset);

typedef struct {
    /* Updated every tick */
    Entity* ent;
    bool is_steam_friend;
    player_info_t pinfo;
    /* TODO: is_a_party_member */

    /* Can be set through playerlist menu */
    bool is_ignored;
    enum EPlayerListPlayerPreset preset;

    /* Should be true before processing the struct */
    bool is_good;
} plist_player_t;

#endif /* PLAYERLIST_H_ */
