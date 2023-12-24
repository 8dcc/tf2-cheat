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

#define SETT_OFF 0

enum ESettingsRage {
    SETT_LEGIT = 1,
    SETT_RAGE  = 2,
};

enum ESettingsFriendly {
    SETT_FRIEND = 1,
    SETT_ENEMY  = 2,
    SETT_ALL    = 3,
};

enum ESettingsBuildings {
    SETT_BTYPE_ALL        = 0,
    SETT_BTYPE_SENTRY     = 1,
    SETT_BTYPE_DISPENSER  = 2,
    SETT_BTYPE_TELEPORTER = 3,
};

enum ESettingsHitboxes {
    SETT_HITBOX_HEAD  = 0,
    SETT_HITBOX_TORSO = 1,
    SETT_HITBOX_ARMS  = 2,
    SETT_HITBOX_LEGS  = 3,
};

typedef struct {
    /* ESP */
    int esp_player;
    int esp_use_team_color;
    int esp_player_box;
    int esp_skeleton;
    int esp_player_health;
    int esp_player_name;
    int esp_player_class;
    int esp_player_weapon;
    int esp_ignore_invisible;

    int esp_building;
    int esp_building_use_team_color;
    int esp_building_type;
    int esp_building_box;
    int esp_building_hp;
    int esp_building_name;

    int esp_sticky;
    int esp_sticky_use_team_color;
    int esp_ammobox;
    int esp_healthpack;

    int chams_player;
    int chams_player_use_team_color;
    int chams_ignorez;
    int chams_local;
    int chams_weapon;
    int chams_hand;

    /* Aim */
    int aim_target_steam_friends;
    int aim_target_invisible;
    int aim_target_invul;

    int aimbot;
    float aim_fov;
    float aim_smooth;
    float aim_deg_threshold;
    int aim_hitbox;
    int aim_silent;
    int aim_on_key;
    int aim_keycode;
    int aim_ignore_walls;
    int aim_autoscope;
    int aim_off_unscoped;
    int aim_off_spectated;
    int aim_draw_fov;

    int meleebot;
    int melee_silent;
    int melee_swing_pred;
    int melee_on_key;
    int melee_keycode;
    int melee_off_spectated;

    int automedigun;
    int automedigun_silent;
    float automedigun_smooth;
    float automedigun_switch_time;

    /* Visuals */
    int remove_scope;
    int custom_fov;
    float custom_fov_deg;
    int custom_fov_scoped;
    int thirdperson;
    int thirdperson_network;
    int draw_velocity;
    int draw_velocity_jump;
    int draw_velocity_vert;
    float draw_velocity_pos;
    int watermark;
    int speclist;

    /* Misc */
    int bhop;
    int autostrafe;
    int aa;
    float aa_pitch;
    float aa_yaw;
    int aa_spin;
    float aa_speed;
    int rocketjump;
    float rocketjump_deg;
    int auto_detonate;
    int auto_detonate_self;
    float auto_detonate_dist;
    int crits_melee;
    int crits_chance;
    int autostab;
    int anim_stab;
    int nopush;
    int antiafk;

    /* Colors */
    struct nk_colorf col_team_red;
    struct nk_colorf col_team_blu;
    struct nk_colorf col_esp_steam_friend;
    struct nk_colorf col_esp_friend;
    struct nk_colorf col_esp_enemy;
    struct nk_colorf col_esp_enemy_build;
    struct nk_colorf col_esp_friend_build;
    struct nk_colorf col_esp_sticky_friend;
    struct nk_colorf col_esp_sticky_enemy;
    struct nk_colorf col_esp_ammobox;
    struct nk_colorf col_esp_healthpack;

    struct nk_colorf col_chams_friend;
    struct nk_colorf col_chams_enemy;
    struct nk_colorf col_chams_local;
    struct nk_colorf col_chams_weapon;
    struct nk_colorf col_chams_hand;

    struct nk_colorf col_aim_fov;
} Settings;

extern Settings settings;

/*----------------------------------------------------------------------------*/

void save_config(const char* filename);
void load_config(const char* filename);

#endif /* SETTINGS_H_ */
