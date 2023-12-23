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
    SETT_FRIENDLY = 1,
    SETT_ENEMY    = 2,
    SETT_ALL      = 3,
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
    /* TODO: Rename */
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

    int sticky_esp;
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
    float aim_deg_threshold;
    int aim_hitbox;
    int aim_silent;
    int aim_on_key;
    int aim_keycode;
    int aim_target_invis;
    int aim_target_invul;
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
    struct nk_colorf col_steam_friend_esp;
    struct nk_colorf col_friend_esp;
    struct nk_colorf col_enemy_esp;
    struct nk_colorf col_enemy_build;
    struct nk_colorf col_friend_build;
    struct nk_colorf col_sticky_friend_esp;
    struct nk_colorf col_sticky_enemy_esp;
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
