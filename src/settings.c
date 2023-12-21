
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>  /* mkdir */
#include <sys/types.h> /* mkdir*/
#include "dependencies/cJSON/cJSON.h"
#include "include/settings.h"

/*----------------------------------------------------------------------------*/

/* Global settings */
Settings settings = {
    /* ESP */
    .player_esp        = SETT_OFF,
    .player_box_esp    = false,
    .skeleton_esp      = false,
    .player_health_esp = false,
    .player_name_esp   = false,
    .player_class_esp  = false,
    .player_weapon_esp = false,

    .building_esp      = SETT_OFF,
    .building_esp_type = SETT_BTYPE_ALL,
    .building_box_esp  = false,
    .building_hp_esp   = false,
    .building_name_esp = false,

    .ammobox_esp    = false,
    .healthpack_esp = false,

    .player_chams  = SETT_OFF,
    .chams_ignorez = false,
    .local_chams   = false,
    .weapon_chams  = false,
    .hand_chams    = false,

    /* Aim */
    .aimbot            = false,
    .aim_fov           = 0.f,
    .aim_smooth        = 1.f,
    .aim_deg_threshold = 1.f,
    .aim_hitbox        = SETT_HITBOX_HEAD,
    .aim_silent        = false,
    .aim_on_key        = false,
    .aim_keycode       = DEFAULT_AIMBOT_KEY,
    .aim_target_invis  = false,
    .aim_target_invul  = false,
    .aim_autoscope     = false,
    .aim_off_unscoped  = false,
    .aim_off_spectated = false,
    .aim_draw_fov      = false,

    .meleebot            = false,
    .melee_silent        = false,
    .melee_swing_pred    = false,
    .melee_on_key        = false,
    .melee_keycode       = DEFAULT_MELEEBOT_KEY,
    .melee_off_spectated = false,

    .automedigun             = false,
    .automedigun_silent      = false,
    .automedigun_smooth      = 0.f,
    .automedigun_switch_time = 1.f,

    /* Visuals */
    .remove_scope        = false,
    .custom_fov          = false,
    .custom_fov_deg      = 90.f,
    .custom_fov_scoped   = false,
    .thirdperson         = false,
    .thirdperson_network = false,
    .draw_velocity       = false,
    .draw_velocity_jump  = false,
    .draw_velocity_vert  = false,
    .draw_velocity_pos   = 51.f,
    .watermark           = true,
    .speclist            = true,

    /* Misc */
    .bhop               = false,
    .autostrafe         = SETT_OFF,
    .aa                 = false,
    .aa_pitch           = 0.f,
    .aa_yaw             = 0.f,
    .aa_spin            = false,
    .aa_speed           = 0.f,
    .rocketjump         = false,
    .rocketjump_deg     = 45.f,
    .auto_detonate      = false,
    .auto_detonate_self = false,
    .auto_detonate_dist = 120.f,
    .crits_melee        = false,
    .crits_chance       = 100,
    .autostab           = false,
    .anim_stab          = false,
    .nopush             = false,
    .antiafk            = false,

    /* Colors */
    .col_steam_friend_esp = (struct nk_colorf){ 0.21f, 0.77f, 0.23f, 1.f },
    .col_friend_esp       = (struct nk_colorf){ 0.05f, 0.47f, 0.95f, 1.f },
    .col_enemy_esp        = (struct nk_colorf){ 0.95f, 0.10f, 0.09f, 1.f },
    .col_friend_build     = (struct nk_colorf){ 0.29f, 0.07f, 0.54f, 1.f },
    .col_enemy_build      = (struct nk_colorf){ 0.90f, 0.31f, 0.00f, 1.f },
    .col_ammobox_esp      = (struct nk_colorf){ 0.55f, 0.43f, 0.38f, 1.f },
    .col_healthpack_esp   = (struct nk_colorf){ 0.40f, 0.73f, 0.41f, 1.f },

    .col_friend_chams = (struct nk_colorf){ 0.47f, 0.77f, 0.90f, 1.f },
    .col_enemy_chams  = (struct nk_colorf){ 0.87f, 0.34f, 0.34f, 1.f },
    .col_local_chams  = (struct nk_colorf){ 0.88f, 0.74f, 0.90f, 1.f },
    .col_weapon_chams = (struct nk_colorf){ 0.80f, 0.57f, 0.84f, 1.f },
    .col_hand_chams   = (struct nk_colorf){ 0.88f, 0.74f, 0.90f, 1.f },

    .col_aim_fov = (struct nk_colorf){ 0.80f, 0.80f, 0.80f, 0.30f },
};

/*----------------------------------------------------------------------------*/

/* Most uppercase macros from this file are in this header to clean up */
#include "include/settings_macros.h"

void save_config(const char* filename) {
    if (!filename || *filename == '\0') {
        fprintf(stderr, "load_config: invalid or empty config name\n");
        return;
    }

    cJSON* json_cfg = cJSON_CreateObject();
    if (!json_cfg)
        SAVE_ABORT("save_config: error creating main json object\n");

    /* Esp */
    JSON_SETTINGS_WRITE_INT(json_cfg, player_esp);
    JSON_SETTINGS_WRITE_INT(json_cfg, player_box_esp);
    JSON_SETTINGS_WRITE_INT(json_cfg, skeleton_esp);
    JSON_SETTINGS_WRITE_INT(json_cfg, player_health_esp);
    JSON_SETTINGS_WRITE_INT(json_cfg, player_name_esp);
    JSON_SETTINGS_WRITE_INT(json_cfg, player_class_esp);
    JSON_SETTINGS_WRITE_INT(json_cfg, player_weapon_esp);
    JSON_SETTINGS_WRITE_INT(json_cfg, building_esp);
    JSON_SETTINGS_WRITE_INT(json_cfg, building_esp_type);
    JSON_SETTINGS_WRITE_INT(json_cfg, building_box_esp);
    JSON_SETTINGS_WRITE_INT(json_cfg, building_hp_esp);
    JSON_SETTINGS_WRITE_INT(json_cfg, building_name_esp);
    JSON_SETTINGS_WRITE_INT(json_cfg, ammobox_esp);
    JSON_SETTINGS_WRITE_INT(json_cfg, healthpack_esp);
    JSON_SETTINGS_WRITE_INT(json_cfg, player_chams);
    JSON_SETTINGS_WRITE_INT(json_cfg, chams_ignorez);
    JSON_SETTINGS_WRITE_INT(json_cfg, local_chams);
    JSON_SETTINGS_WRITE_INT(json_cfg, weapon_chams);
    JSON_SETTINGS_WRITE_INT(json_cfg, hand_chams);

    /* Aim */
    JSON_SETTINGS_WRITE_INT(json_cfg, aimbot);
    JSON_SETTINGS_WRITE_FLOAT(json_cfg, aim_fov);
    JSON_SETTINGS_WRITE_FLOAT(json_cfg, aim_smooth);
    JSON_SETTINGS_WRITE_FLOAT(json_cfg, aim_deg_threshold);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_hitbox);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_silent);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_on_key);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_keycode);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_target_invis);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_target_invul);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_autoscope);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_off_unscoped);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_off_spectated);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_draw_fov);
    JSON_SETTINGS_WRITE_INT(json_cfg, meleebot);
    JSON_SETTINGS_WRITE_INT(json_cfg, melee_silent);
    JSON_SETTINGS_WRITE_INT(json_cfg, melee_swing_pred);
    JSON_SETTINGS_WRITE_INT(json_cfg, melee_on_key);
    JSON_SETTINGS_WRITE_INT(json_cfg, melee_keycode);
    JSON_SETTINGS_WRITE_INT(json_cfg, melee_off_spectated);
    JSON_SETTINGS_WRITE_INT(json_cfg, automedigun);
    JSON_SETTINGS_WRITE_INT(json_cfg, automedigun_silent);
    JSON_SETTINGS_WRITE_FLOAT(json_cfg, automedigun_smooth);
    JSON_SETTINGS_WRITE_FLOAT(json_cfg, automedigun_switch_time);

    /* Visuals */
    JSON_SETTINGS_WRITE_INT(json_cfg, remove_scope);
    JSON_SETTINGS_WRITE_INT(json_cfg, custom_fov);
    JSON_SETTINGS_WRITE_FLOAT(json_cfg, custom_fov_deg);
    JSON_SETTINGS_WRITE_INT(json_cfg, custom_fov_scoped);
    JSON_SETTINGS_WRITE_INT(json_cfg, thirdperson);
    JSON_SETTINGS_WRITE_INT(json_cfg, thirdperson_network);
    JSON_SETTINGS_WRITE_INT(json_cfg, draw_velocity);
    JSON_SETTINGS_WRITE_INT(json_cfg, draw_velocity_jump);
    JSON_SETTINGS_WRITE_INT(json_cfg, draw_velocity_vert);
    JSON_SETTINGS_WRITE_INT(json_cfg, draw_velocity_pos);
    JSON_SETTINGS_WRITE_INT(json_cfg, watermark);
    JSON_SETTINGS_WRITE_INT(json_cfg, speclist);

    /* Misc */
    JSON_SETTINGS_WRITE_INT(json_cfg, bhop);
    JSON_SETTINGS_WRITE_INT(json_cfg, autostrafe);
    JSON_SETTINGS_WRITE_INT(json_cfg, aa);
    JSON_SETTINGS_WRITE_FLOAT(json_cfg, aa_pitch);
    JSON_SETTINGS_WRITE_FLOAT(json_cfg, aa_yaw);
    JSON_SETTINGS_WRITE_INT(json_cfg, aa_spin);
    JSON_SETTINGS_WRITE_FLOAT(json_cfg, aa_speed);
    JSON_SETTINGS_WRITE_INT(json_cfg, rocketjump);
    JSON_SETTINGS_WRITE_FLOAT(json_cfg, rocketjump_deg);
    JSON_SETTINGS_WRITE_INT(json_cfg, auto_detonate);
    JSON_SETTINGS_WRITE_INT(json_cfg, auto_detonate_self);
    JSON_SETTINGS_WRITE_FLOAT(json_cfg, auto_detonate_dist);
    JSON_SETTINGS_WRITE_INT(json_cfg, crits_melee);
    JSON_SETTINGS_WRITE_INT(json_cfg, crits_chance);
    JSON_SETTINGS_WRITE_INT(json_cfg, autostab);
    JSON_SETTINGS_WRITE_INT(json_cfg, anim_stab);
    JSON_SETTINGS_WRITE_INT(json_cfg, nopush);
    JSON_SETTINGS_WRITE_INT(json_cfg, antiafk);

    /* Cols */
    JSON_SETTINGS_WRITE_COL(json_cfg, col_steam_friend_esp);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_friend_esp);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_enemy_esp);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_friend_build);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_enemy_build);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_ammobox_esp);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_healthpack_esp);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_friend_chams);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_enemy_chams);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_local_chams);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_weapon_chams);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_hand_chams);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_aim_fov);

    /* Convert filled json object to string */
    char* json_cfg_str = cJSON_Print(json_cfg);
    if (!json_cfg_str)
        SAVE_ABORT("save_config: error converting main json object to "
                   "string\n");

    /* Create the config folder */
    mkdir(CONFIG_FOLDER, 0755);

    /* Get path of filename inside CONFIG_FOLDER */
    char* filepath =
      calloc(strlen(filename) + sizeof(CONFIG_FOLDER), sizeof(char));
    strcpy(&filepath[0], CONFIG_FOLDER);
    strcpy(&filepath[sizeof(CONFIG_FOLDER) - 1], filename);

    FILE* fd = fopen(filepath, "w+");
    if (!fd) {
        free(json_cfg_str);
        SAVE_ABORT("save_config: error opening \"%s\"\n", filename);
    }

    fprintf(fd, "%s\n", json_cfg_str);
    fclose(fd);

    free(json_cfg_str);
    cJSON_Delete(json_cfg);

    printf("[Enoch] Saved config %s\n", filename);
}

/*----------------------------------------------------------------------------*/

void load_config(const char* filename) {
    if (!filename || *filename == '\0') {
        fprintf(stderr, "load_config: invalid or empty config name\n");
        return;
    }

    char* filepath =
      calloc(strlen(filename) + sizeof(CONFIG_FOLDER), sizeof(char));
    strcpy(&filepath[0], CONFIG_FOLDER);
    strcpy(&filepath[sizeof(CONFIG_FOLDER) - 1], filename);

    FILE* fd = fopen(filepath, "r");
    if (!fd) {
        fprintf(stderr, "load_config: %s\n", cJSON_GetErrorPtr());
        return;
    }

    /* Get file size for allocating (without EOF) */
    fseek(fd, 0, SEEK_END);
    int file_sz = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    /* Allocate string for contents. Add 1 to length for '\0' */
    char* json_cfg_str = malloc(file_sz + 1);

    /* Read contents of config file */
    char c;
    int i = 0;
    while ((c = fgetc(fd)) != EOF)
        json_cfg_str[i++] = c;
    json_cfg_str[i] = '\0';

    fclose(fd);

    /* Parse json */
    cJSON* json_cfg = cJSON_Parse(json_cfg_str);
    if (!json_cfg)
        LOAD_PRINT_ERROR();

    /* Esp */
    JSON_SETTINGS_READ_INT(json_cfg, player_esp);
    JSON_SETTINGS_READ_INT(json_cfg, player_box_esp);
    JSON_SETTINGS_READ_INT(json_cfg, skeleton_esp);
    JSON_SETTINGS_READ_INT(json_cfg, player_health_esp);
    JSON_SETTINGS_READ_INT(json_cfg, player_name_esp);
    JSON_SETTINGS_READ_INT(json_cfg, player_class_esp);
    JSON_SETTINGS_READ_INT(json_cfg, player_weapon_esp);
    JSON_SETTINGS_READ_INT(json_cfg, building_esp);
    JSON_SETTINGS_READ_INT(json_cfg, building_esp_type);
    JSON_SETTINGS_READ_INT(json_cfg, building_box_esp);
    JSON_SETTINGS_READ_INT(json_cfg, building_hp_esp);
    JSON_SETTINGS_READ_INT(json_cfg, building_name_esp);
    JSON_SETTINGS_READ_INT(json_cfg, ammobox_esp);
    JSON_SETTINGS_READ_INT(json_cfg, healthpack_esp);
    JSON_SETTINGS_READ_INT(json_cfg, player_chams);
    JSON_SETTINGS_READ_INT(json_cfg, chams_ignorez);
    JSON_SETTINGS_READ_INT(json_cfg, local_chams);
    JSON_SETTINGS_READ_INT(json_cfg, weapon_chams);
    JSON_SETTINGS_READ_INT(json_cfg, hand_chams);

    /* Aim */
    JSON_SETTINGS_READ_INT(json_cfg, aimbot);
    JSON_SETTINGS_READ_FLOAT(json_cfg, aim_fov);
    JSON_SETTINGS_READ_FLOAT(json_cfg, aim_smooth);
    JSON_SETTINGS_READ_FLOAT(json_cfg, aim_deg_threshold);
    JSON_SETTINGS_READ_INT(json_cfg, aim_hitbox);
    JSON_SETTINGS_READ_INT(json_cfg, aim_silent);
    JSON_SETTINGS_READ_INT(json_cfg, aim_on_key);
    JSON_SETTINGS_READ_INT(json_cfg, aim_keycode);
    JSON_SETTINGS_READ_INT(json_cfg, aim_target_invis);
    JSON_SETTINGS_READ_INT(json_cfg, aim_target_invul);
    JSON_SETTINGS_READ_INT(json_cfg, aim_autoscope);
    JSON_SETTINGS_READ_INT(json_cfg, aim_off_unscoped);
    JSON_SETTINGS_READ_INT(json_cfg, aim_off_spectated);
    JSON_SETTINGS_READ_INT(json_cfg, aim_draw_fov);
    JSON_SETTINGS_READ_INT(json_cfg, meleebot);
    JSON_SETTINGS_READ_INT(json_cfg, melee_silent);
    JSON_SETTINGS_READ_INT(json_cfg, melee_swing_pred);
    JSON_SETTINGS_READ_INT(json_cfg, melee_on_key);
    JSON_SETTINGS_READ_INT(json_cfg, melee_keycode);
    JSON_SETTINGS_READ_INT(json_cfg, melee_off_spectated);
    JSON_SETTINGS_READ_INT(json_cfg, automedigun);
    JSON_SETTINGS_READ_INT(json_cfg, automedigun_silent);
    JSON_SETTINGS_READ_FLOAT(json_cfg, automedigun_smooth);
    JSON_SETTINGS_READ_FLOAT(json_cfg, automedigun_switch_time);

    /* Visuals */
    JSON_SETTINGS_READ_INT(json_cfg, remove_scope);
    JSON_SETTINGS_READ_INT(json_cfg, custom_fov);
    JSON_SETTINGS_READ_FLOAT(json_cfg, custom_fov_deg);
    JSON_SETTINGS_READ_INT(json_cfg, custom_fov_scoped);
    JSON_SETTINGS_READ_INT(json_cfg, thirdperson);
    JSON_SETTINGS_READ_INT(json_cfg, thirdperson_network);
    JSON_SETTINGS_READ_INT(json_cfg, draw_velocity);
    JSON_SETTINGS_READ_INT(json_cfg, draw_velocity_jump);
    JSON_SETTINGS_READ_INT(json_cfg, draw_velocity_vert);
    JSON_SETTINGS_READ_INT(json_cfg, draw_velocity_pos);
    JSON_SETTINGS_READ_INT(json_cfg, watermark);
    JSON_SETTINGS_READ_INT(json_cfg, speclist);

    /* Misc */
    JSON_SETTINGS_READ_INT(json_cfg, bhop);
    JSON_SETTINGS_READ_INT(json_cfg, autostrafe);
    JSON_SETTINGS_READ_INT(json_cfg, aa);
    JSON_SETTINGS_READ_FLOAT(json_cfg, aa_pitch);
    JSON_SETTINGS_READ_FLOAT(json_cfg, aa_yaw);
    JSON_SETTINGS_READ_INT(json_cfg, aa_spin);
    JSON_SETTINGS_READ_FLOAT(json_cfg, aa_speed);
    JSON_SETTINGS_READ_INT(json_cfg, rocketjump);
    JSON_SETTINGS_READ_FLOAT(json_cfg, rocketjump_deg);
    JSON_SETTINGS_READ_INT(json_cfg, auto_detonate);
    JSON_SETTINGS_READ_INT(json_cfg, auto_detonate_self);
    JSON_SETTINGS_READ_FLOAT(json_cfg, auto_detonate_dist);
    JSON_SETTINGS_READ_INT(json_cfg, crits_melee);
    JSON_SETTINGS_READ_INT(json_cfg, crits_chance);
    JSON_SETTINGS_READ_INT(json_cfg, autostab);
    JSON_SETTINGS_READ_INT(json_cfg, anim_stab);
    JSON_SETTINGS_READ_INT(json_cfg, nopush);
    JSON_SETTINGS_READ_INT(json_cfg, antiafk);

    /* Colors */
    JSON_SETTINGS_READ_COL(json_cfg, col_steam_friend_esp);
    JSON_SETTINGS_READ_COL(json_cfg, col_friend_esp);
    JSON_SETTINGS_READ_COL(json_cfg, col_enemy_esp);
    JSON_SETTINGS_READ_COL(json_cfg, col_friend_build);
    JSON_SETTINGS_READ_COL(json_cfg, col_enemy_build);
    JSON_SETTINGS_READ_COL(json_cfg, col_ammobox_esp);
    JSON_SETTINGS_READ_COL(json_cfg, col_healthpack_esp);
    JSON_SETTINGS_READ_COL(json_cfg, col_friend_chams);
    JSON_SETTINGS_READ_COL(json_cfg, col_enemy_chams);
    JSON_SETTINGS_READ_COL(json_cfg, col_local_chams);
    JSON_SETTINGS_READ_COL(json_cfg, col_weapon_chams);
    JSON_SETTINGS_READ_COL(json_cfg, col_hand_chams);
    JSON_SETTINGS_READ_COL(json_cfg, col_aim_fov);

    free(json_cfg_str);
    cJSON_Delete(json_cfg);

    printf("[Enoch] Loaded config %s\n", filename);
}
