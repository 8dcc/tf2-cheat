
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
    .esp_player           = SETT_OFF,
    .esp_use_team_color   = false,
    .esp_player_box       = false,
    .esp_skeleton         = false,
    .esp_player_health    = false,
    .esp_player_name      = false,
    .esp_player_class     = false,
    .esp_player_weapon    = false,
    .esp_ignore_invisible = false,

    .esp_building                = SETT_OFF,
    .esp_building_use_team_color = false,
    .esp_building_type           = SETT_BTYPE_ALL,
    .esp_building_box            = false,
    .esp_building_hp             = false,
    .esp_building_name           = false,

    .esp_sticky                = false,
    .esp_sticky_use_team_color = false,
    .esp_ammobox               = false,
    .esp_healthpack            = false,

    .chams_player                = SETT_OFF,
    .chams_player_use_team_color = false,
    .chams_ignorez               = false,
    .chams_local                 = false,
    .chams_weapon                = false,
    .chams_hand                  = false,

    /* Aim */
    .aim_target_steam_friends = false,
    .aim_target_invisible     = false,
    .aim_target_invul         = false,

    .aimbot            = false,
    .aim_fov           = 0.f,
    .aim_smooth        = 1.f,
    .aim_deg_threshold = 1.f,
    .aim_hitbox        = SETT_HITBOX_HEAD,
    .aim_silent        = false,
    .aim_on_key        = false,
    .aim_keycode       = DEFAULT_AIMBOT_KEY,
    .aim_ignore_walls  = false,
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
    .col_team_red          = (struct nk_colorf){ 0.62f, 0.19f, 0.18f, 1.f },
    .col_team_blu          = (struct nk_colorf){ 0.22f, 0.36f, 0.47f, 1.f },
    .col_esp_steam_friend  = (struct nk_colorf){ 0.21f, 0.77f, 0.23f, 1.f },
    .col_esp_friend        = (struct nk_colorf){ 0.05f, 0.47f, 0.95f, 1.f },
    .col_esp_enemy         = (struct nk_colorf){ 0.95f, 0.10f, 0.09f, 1.f },
    .col_esp_friend_build  = (struct nk_colorf){ 0.29f, 0.07f, 0.54f, 1.f },
    .col_esp_enemy_build   = (struct nk_colorf){ 0.90f, 0.31f, 0.00f, 1.f },
    .col_esp_sticky_friend = (struct nk_colorf){ 0.05f, 0.47f, 0.95f, 1.f },
    .col_esp_sticky_enemy  = (struct nk_colorf){ 0.95f, 0.10f, 0.09f, 1.f },
    .col_esp_ammobox       = (struct nk_colorf){ 0.55f, 0.43f, 0.38f, 1.f },
    .col_esp_healthpack    = (struct nk_colorf){ 0.40f, 0.73f, 0.41f, 1.f },

    .col_chams_friend = (struct nk_colorf){ 0.47f, 0.77f, 0.90f, 1.f },
    .col_chams_enemy  = (struct nk_colorf){ 0.87f, 0.34f, 0.34f, 1.f },
    .col_chams_local  = (struct nk_colorf){ 0.88f, 0.74f, 0.90f, 1.f },
    .col_chams_weapon = (struct nk_colorf){ 0.80f, 0.57f, 0.84f, 1.f },
    .col_chams_hand   = (struct nk_colorf){ 0.88f, 0.74f, 0.90f, 1.f },

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
    JSON_SETTINGS_WRITE_INT(json_cfg, esp_player);
    JSON_SETTINGS_WRITE_INT(json_cfg, esp_use_team_color);
    JSON_SETTINGS_WRITE_INT(json_cfg, esp_player_box);
    JSON_SETTINGS_WRITE_INT(json_cfg, esp_skeleton);
    JSON_SETTINGS_WRITE_INT(json_cfg, esp_player_health);
    JSON_SETTINGS_WRITE_INT(json_cfg, esp_player_name);
    JSON_SETTINGS_WRITE_INT(json_cfg, esp_player_class);
    JSON_SETTINGS_WRITE_INT(json_cfg, esp_player_weapon);
    JSON_SETTINGS_WRITE_INT(json_cfg, esp_ignore_invisible);
    JSON_SETTINGS_WRITE_INT(json_cfg, esp_building);
    JSON_SETTINGS_WRITE_INT(json_cfg, esp_building_use_team_color);
    JSON_SETTINGS_WRITE_INT(json_cfg, esp_building_type);
    JSON_SETTINGS_WRITE_INT(json_cfg, esp_building_box);
    JSON_SETTINGS_WRITE_INT(json_cfg, esp_building_hp);
    JSON_SETTINGS_WRITE_INT(json_cfg, esp_building_name);
    JSON_SETTINGS_WRITE_INT(json_cfg, esp_sticky);
    JSON_SETTINGS_WRITE_INT(json_cfg, esp_sticky_use_team_color);
    JSON_SETTINGS_WRITE_INT(json_cfg, esp_ammobox);
    JSON_SETTINGS_WRITE_INT(json_cfg, esp_healthpack);
    JSON_SETTINGS_WRITE_INT(json_cfg, chams_player);
    JSON_SETTINGS_WRITE_INT(json_cfg, chams_player_use_team_color);
    JSON_SETTINGS_WRITE_INT(json_cfg, chams_ignorez);
    JSON_SETTINGS_WRITE_INT(json_cfg, chams_local);
    JSON_SETTINGS_WRITE_INT(json_cfg, chams_weapon);
    JSON_SETTINGS_WRITE_INT(json_cfg, chams_hand);

    /* Aim */
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_target_steam_friends);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_target_invisible);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_target_invul);
    JSON_SETTINGS_WRITE_INT(json_cfg, aimbot);
    JSON_SETTINGS_WRITE_FLOAT(json_cfg, aim_fov);
    JSON_SETTINGS_WRITE_FLOAT(json_cfg, aim_smooth);
    JSON_SETTINGS_WRITE_FLOAT(json_cfg, aim_deg_threshold);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_hitbox);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_silent);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_on_key);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_keycode);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_ignore_walls);
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

    /* Colors */
    JSON_SETTINGS_WRITE_COL(json_cfg, col_team_red);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_team_blu);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_esp_steam_friend);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_esp_friend);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_esp_enemy);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_esp_friend_build);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_esp_enemy_build);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_esp_sticky_friend);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_esp_sticky_enemy);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_esp_ammobox);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_esp_healthpack);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_chams_friend);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_chams_enemy);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_chams_local);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_chams_weapon);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_chams_hand);
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
    JSON_SETTINGS_READ_INT(json_cfg, esp_player);
    JSON_SETTINGS_WRITE_INT(json_cfg, esp_use_team_color);
    JSON_SETTINGS_READ_INT(json_cfg, esp_player_box);
    JSON_SETTINGS_READ_INT(json_cfg, esp_skeleton);
    JSON_SETTINGS_READ_INT(json_cfg, esp_player_health);
    JSON_SETTINGS_READ_INT(json_cfg, esp_player_name);
    JSON_SETTINGS_READ_INT(json_cfg, esp_player_class);
    JSON_SETTINGS_READ_INT(json_cfg, esp_player_weapon);
    JSON_SETTINGS_READ_INT(json_cfg, esp_ignore_invisible);
    JSON_SETTINGS_READ_INT(json_cfg, esp_building);
    JSON_SETTINGS_READ_INT(json_cfg, esp_building_use_team_color);
    JSON_SETTINGS_READ_INT(json_cfg, esp_building_type);
    JSON_SETTINGS_READ_INT(json_cfg, esp_building_box);
    JSON_SETTINGS_READ_INT(json_cfg, esp_building_hp);
    JSON_SETTINGS_READ_INT(json_cfg, esp_building_name);
    JSON_SETTINGS_READ_INT(json_cfg, esp_sticky);
    JSON_SETTINGS_READ_INT(json_cfg, esp_sticky_use_team_color);
    JSON_SETTINGS_READ_INT(json_cfg, esp_ammobox);
    JSON_SETTINGS_READ_INT(json_cfg, esp_healthpack);
    JSON_SETTINGS_READ_INT(json_cfg, chams_player);
    JSON_SETTINGS_READ_INT(json_cfg, chams_player_use_team_color);
    JSON_SETTINGS_READ_INT(json_cfg, chams_ignorez);
    JSON_SETTINGS_READ_INT(json_cfg, chams_local);
    JSON_SETTINGS_READ_INT(json_cfg, chams_weapon);
    JSON_SETTINGS_READ_INT(json_cfg, chams_hand);

    /* Aim */
    JSON_SETTINGS_READ_INT(json_cfg, aim_target_steam_friends);
    JSON_SETTINGS_READ_INT(json_cfg, aim_target_invisible);
    JSON_SETTINGS_READ_INT(json_cfg, aim_target_invul);
    JSON_SETTINGS_READ_INT(json_cfg, aimbot);
    JSON_SETTINGS_READ_FLOAT(json_cfg, aim_fov);
    JSON_SETTINGS_READ_FLOAT(json_cfg, aim_smooth);
    JSON_SETTINGS_READ_FLOAT(json_cfg, aim_deg_threshold);
    JSON_SETTINGS_READ_INT(json_cfg, aim_hitbox);
    JSON_SETTINGS_READ_INT(json_cfg, aim_silent);
    JSON_SETTINGS_READ_INT(json_cfg, aim_on_key);
    JSON_SETTINGS_READ_INT(json_cfg, aim_keycode);
    JSON_SETTINGS_READ_INT(json_cfg, aim_ignore_walls);
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
    JSON_SETTINGS_READ_COL(json_cfg, col_team_red);
    JSON_SETTINGS_READ_COL(json_cfg, col_team_blu);
    JSON_SETTINGS_READ_COL(json_cfg, col_esp_steam_friend);
    JSON_SETTINGS_READ_COL(json_cfg, col_esp_friend);
    JSON_SETTINGS_READ_COL(json_cfg, col_esp_enemy);
    JSON_SETTINGS_READ_COL(json_cfg, col_esp_friend_build);
    JSON_SETTINGS_READ_COL(json_cfg, col_esp_enemy_build);
    JSON_SETTINGS_READ_COL(json_cfg, col_esp_sticky_friend);
    JSON_SETTINGS_READ_COL(json_cfg, col_esp_sticky_enemy);
    JSON_SETTINGS_READ_COL(json_cfg, col_esp_ammobox);
    JSON_SETTINGS_READ_COL(json_cfg, col_esp_healthpack);
    JSON_SETTINGS_READ_COL(json_cfg, col_chams_friend);
    JSON_SETTINGS_READ_COL(json_cfg, col_chams_enemy);
    JSON_SETTINGS_READ_COL(json_cfg, col_chams_local);
    JSON_SETTINGS_READ_COL(json_cfg, col_chams_weapon);
    JSON_SETTINGS_READ_COL(json_cfg, col_chams_hand);
    JSON_SETTINGS_READ_COL(json_cfg, col_aim_fov);

    free(json_cfg_str);
    cJSON_Delete(json_cfg);

    printf("[Enoch] Loaded config %s\n", filename);
}
