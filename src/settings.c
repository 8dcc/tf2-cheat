
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
    .player_esp        = OFF,
    .player_box_esp    = false,
    .skeleton_esp      = false,
    .player_health_esp = false,
    .player_name_esp   = false,
    .player_class_esp  = false,
    .player_weapon_esp = false,

    .building_esp      = OFF,
    .building_esp_type = BTYPE_ALL,
    .building_box_esp  = false,
    .building_hp_esp   = false,
    .building_name_esp = false,

    .ammobox_esp    = false,
    .healthpack_esp = false,

    /* Aim */
    .aimbot              = false,
    .aim_fov             = 0.f,
    .aim_smooth          = 1.f,
    .aim_hitbox          = SETTING_HITBOX_HEAD,
    .aim_silent          = false,
    .aim_shoot_if_target = false,
    .aim_ignore_visible  = false,
    .aim_draw_fov        = false,
    .aim_off_spectated   = false,

    /* Misc */
    .bhop       = false,
    .autostrafe = OFF,
    .rocketjump = false,
    .autostab   = false,
    .watermark  = true,
    .speclist   = true,

    /* Colors */
    .col_friend_esp     = (struct nk_colorf){ 0.05f, 0.47f, 0.95f, 1.f },
    .col_enemy_esp      = (struct nk_colorf){ 0.95f, 0.10f, 0.09f, 1.f },
    .col_friend_build   = (struct nk_colorf){ 0.29f, 0.07f, 0.54f, 1.f },
    .col_enemy_build    = (struct nk_colorf){ 0.90f, 0.31f, 0.00f, 1.f },
    .col_ammobox_esp    = (struct nk_colorf){ 0.55f, 0.43f, 0.38f, 1.f },
    .col_healthpack_esp = (struct nk_colorf){ 0.40f, 0.73f, 0.41f, 1.f },
    .col_aim_fov        = (struct nk_colorf){ 0.80f, 0.80f, 0.80f, 0.30f },
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

    JSON_SETTINGS_WRITE_INT(json_cfg, aimbot);
    JSON_SETTINGS_WRITE_FLOAT(json_cfg, aim_fov);
    JSON_SETTINGS_WRITE_FLOAT(json_cfg, aim_smooth);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_hitbox);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_silent);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_shoot_if_target);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_ignore_visible);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_draw_fov);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_off_spectated);

    JSON_SETTINGS_WRITE_INT(json_cfg, bhop);
    JSON_SETTINGS_WRITE_INT(json_cfg, autostrafe);
    JSON_SETTINGS_WRITE_INT(json_cfg, rocketjump);
    JSON_SETTINGS_WRITE_INT(json_cfg, autostab);
    JSON_SETTINGS_WRITE_INT(json_cfg, watermark);
    JSON_SETTINGS_WRITE_INT(json_cfg, speclist);

    JSON_SETTINGS_WRITE_COL(json_cfg, col_friend_esp);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_enemy_esp);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_friend_build);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_enemy_build);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_ammobox_esp);
    JSON_SETTINGS_WRITE_COL(json_cfg, col_healthpack_esp);
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

    JSON_SETTINGS_READ_INT(json_cfg, aimbot);
    JSON_SETTINGS_READ_FLOAT(json_cfg, aim_fov);
    JSON_SETTINGS_READ_FLOAT(json_cfg, aim_smooth);
    JSON_SETTINGS_READ_INT(json_cfg, aim_hitbox);
    JSON_SETTINGS_READ_INT(json_cfg, aim_silent);
    JSON_SETTINGS_READ_INT(json_cfg, aim_shoot_if_target);
    JSON_SETTINGS_WRITE_INT(json_cfg, aim_ignore_visible);
    JSON_SETTINGS_READ_INT(json_cfg, aim_draw_fov);
    JSON_SETTINGS_READ_INT(json_cfg, aim_off_spectated);

    JSON_SETTINGS_READ_INT(json_cfg, bhop);
    JSON_SETTINGS_READ_INT(json_cfg, autostrafe);
    JSON_SETTINGS_READ_INT(json_cfg, rocketjump);
    JSON_SETTINGS_READ_INT(json_cfg, autostab);
    JSON_SETTINGS_READ_INT(json_cfg, watermark);
    JSON_SETTINGS_READ_INT(json_cfg, speclist);

    JSON_SETTINGS_READ_COL(json_cfg, col_friend_esp);
    JSON_SETTINGS_READ_COL(json_cfg, col_enemy_esp);
    JSON_SETTINGS_READ_COL(json_cfg, col_friend_build);
    JSON_SETTINGS_READ_COL(json_cfg, col_enemy_build);
    JSON_SETTINGS_READ_COL(json_cfg, col_ammobox_esp);
    JSON_SETTINGS_READ_COL(json_cfg, col_healthpack_esp);
    JSON_SETTINGS_READ_COL(json_cfg, col_aim_fov);

    free(json_cfg_str);
    cJSON_Delete(json_cfg);

    printf("[Enoch] Loaded config %s\n", filename);
}
