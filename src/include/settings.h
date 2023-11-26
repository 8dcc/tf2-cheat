#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "menu.h"

#define CONFIG_FOLDER "enoch-configs/"

/*
 * Page up for aimbots by default. Currently the only way to change the keybinds
 * is either to change this define or change the value in each config direclty
 * from the json. For keys, see:
 *   https://github.com/8dcc/tf2-cheat/blob/54e83f4b785723939380de4e52040ef3d85d5a30/src/dependencies/nuklear/nuklear.h#L305-L340
 */
#define DEFAULT_AIMBOT_KEY   NK_KEY_SCROLL_UP
#define DEFAULT_MELEEBOT_KEY NK_KEY_SCROLL_UP

enum settings_rage {
    SETT_OFF   = 0,
    SETT_LEGIT = 1,
    SETT_RAGE  = 2,
};

enum settings_friendly {
    SETT_FRIENDLY = 1,
    SETT_ENEMY    = 2,
    SETT_ALL      = 3,
};

enum settings_building_type {
    SETT_BTYPE_ALL        = 0,
    SETT_BTYPE_SENTRY     = 1,
    SETT_BTYPE_DISPENSER  = 2,
    SETT_BTYPE_TELEPORTER = 3,
};

enum settings_selected_hitbox {
    SETT_HITBOX_HEAD = 0,
    SETT_HITBOX_TORSO,
    SETT_HITBOX_ARMS,
    SETT_HITBOX_LEGS,
};

typedef struct {
    /* ESP */
    int player_esp;
    int player_box_esp;
    int skeleton_esp;
    int player_health_esp;
    int player_name_esp;
    int player_class_esp;
    int player_weapon_esp;

    int building_esp;
    int building_esp_type;
    int building_box_esp;
    int building_hp_esp;
    int building_name_esp;

    int ammobox_esp;
    int healthpack_esp;

    int player_chams;
    int chams_ignorez;
    int local_chams;
    int weapon_chams;
    int hand_chams;

    /* Aim */
    int aimbot;
    float aim_fov;
    float aim_smooth;
    int aim_hitbox;
    int aim_silent;
    int aim_on_key;
    int aim_keycode;
    int aim_ignore_visible;
    int aim_draw_fov;
    int aim_off_spectated;

    int meleebot;
    int melee_silent;
    int melee_swing_pred;
    int melee_on_key;
    int melee_keycode;
    int melee_off_spectated;

    /* Misc */
    int bhop;
    int autostrafe;
    int thirdperson;
    int remove_scope;
    int autostab;
    int anim_stab;
    int automedigun;
    int automedigun_silent;
    float automedigun_smooth;
    float automedigun_switch_time;
    int rocketjump;
    float rocketjump_deg;
    int watermark;
    int speclist;

    /* Colors */
    struct nk_colorf col_steam_friend_esp;
    struct nk_colorf col_friend_esp;
    struct nk_colorf col_enemy_esp;
    struct nk_colorf col_enemy_build;
    struct nk_colorf col_friend_build;
    struct nk_colorf col_ammobox_esp;
    struct nk_colorf col_healthpack_esp;

    struct nk_colorf col_friend_chams;
    struct nk_colorf col_enemy_chams;
    struct nk_colorf col_local_chams;
    struct nk_colorf col_weapon_chams;
    struct nk_colorf col_hand_chams;

    struct nk_colorf col_aim_fov;
} Settings;

extern Settings settings;

/*----------------------------------------------------------------------------*/

void save_config(const char* filename);
void load_config(const char* filename);

#endif /* SETTINGS_H_ */
