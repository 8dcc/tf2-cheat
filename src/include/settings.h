#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "menu.h"

#define OFF 0

enum settings_rage {
    LEGIT = 1,
    RAGE  = 2,
};

enum settings_friendly {
    FRIENDLY = 1,
    ENEMY    = 2,
    ALL      = 3,
};

typedef struct {
    /* ESP */
    int player_esp;
    int box_esp;
    int skeleton_esp;
    int player_health_esp;
    int name_esp;
    int class_esp;
    int weapon_esp;
    int ammobox_esp;
    int healthpack_esp;

    /* Movement */
    int bhop;
    int autostrafe;

    /* Misc */
    int watermark;
    int speclist;
    int autostab;

    /* Colors */
    struct nk_colorf col_friend_esp;
    struct nk_colorf col_enemy_esp;
    struct nk_colorf col_ammobox_esp;
    struct nk_colorf col_healthpack_esp;
} Settings;

extern Settings settings;

#endif /* SETTINGS_H_ */
