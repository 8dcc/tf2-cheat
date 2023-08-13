
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>  /* mkdir */
#include <sys/types.h> /* mkdir*/
#include <cjson/cJSON.h>
#include "include/settings.h"

#define CONFIG_FOLDER "enoch-configs/"

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

    /* Misc */
    .bhop       = false,
    .autostrafe = OFF,
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
};

/*----------------------------------------------------------------------------*/

#define SAVE_ABORT(...)               \
    {                                 \
        fprintf(stderr, __VA_ARGS__); \
        cJSON_Delete(json_cfg);       \
        return;                       \
    }

#define JSON_ADD_INT_FROM_SETTINGS(PARENT, ITEM)                \
    if (!cJSON_AddNumberToObject(PARENT, #ITEM, settings.ITEM)) \
        SAVE_ABORT("save_config: error saving \"settings.%s\"\n", #ITEM);

/* Create json_ITEM object with rbga float values, add that object to PARENT */
#define JSON_ADD_COL_FROM_SETTINGS(PARENT, ITEM)                     \
    cJSON* json_##ITEM = cJSON_CreateObject();                       \
    if (!cJSON_AddNumberToObject(json_##ITEM, "r", settings.ITEM.r)) \
        SAVE_ABORT("save_config: error saving red value for "        \
                   "\"settings.%s\"\n",                              \
                   #ITEM);                                           \
    if (!cJSON_AddNumberToObject(json_##ITEM, "g", settings.ITEM.g)) \
        SAVE_ABORT("save_config: error saving red value for "        \
                   "\"settings.%s\"\n",                              \
                   #ITEM);                                           \
    if (!cJSON_AddNumberToObject(json_##ITEM, "b", settings.ITEM.b)) \
        SAVE_ABORT("save_config: error saving red value for "        \
                   "\"settings.%s\"\n",                              \
                   #ITEM);                                           \
    if (!cJSON_AddNumberToObject(json_##ITEM, "a", settings.ITEM.a)) \
        SAVE_ABORT("save_config: error saving red value for "        \
                   "\"settings.%s\"\n",                              \
                   #ITEM);                                           \
    if (!cJSON_AddItemToObject(PARENT, #ITEM, json_##ITEM))          \
        SAVE_ABORT("save_config: error adding rgba object for "      \
                   "\"settings.%s\"\n",                              \
                   #ITEM);

void save_config(const char* filename) {
    cJSON* json_cfg = cJSON_CreateObject();
    if (!json_cfg)
        SAVE_ABORT("save_config: error creating main json object\n");

    JSON_ADD_INT_FROM_SETTINGS(json_cfg, player_esp);
    JSON_ADD_INT_FROM_SETTINGS(json_cfg, player_box_esp);
    JSON_ADD_INT_FROM_SETTINGS(json_cfg, skeleton_esp);
    JSON_ADD_INT_FROM_SETTINGS(json_cfg, player_health_esp);
    JSON_ADD_INT_FROM_SETTINGS(json_cfg, player_name_esp);
    JSON_ADD_INT_FROM_SETTINGS(json_cfg, player_class_esp);
    JSON_ADD_INT_FROM_SETTINGS(json_cfg, player_weapon_esp);
    JSON_ADD_INT_FROM_SETTINGS(json_cfg, building_esp);
    JSON_ADD_INT_FROM_SETTINGS(json_cfg, building_esp_type);
    JSON_ADD_INT_FROM_SETTINGS(json_cfg, building_box_esp);
    JSON_ADD_INT_FROM_SETTINGS(json_cfg, building_hp_esp);
    JSON_ADD_INT_FROM_SETTINGS(json_cfg, building_name_esp);
    JSON_ADD_INT_FROM_SETTINGS(json_cfg, ammobox_esp);
    JSON_ADD_INT_FROM_SETTINGS(json_cfg, healthpack_esp);

    JSON_ADD_INT_FROM_SETTINGS(json_cfg, bhop);
    JSON_ADD_INT_FROM_SETTINGS(json_cfg, autostrafe);
    JSON_ADD_INT_FROM_SETTINGS(json_cfg, autostab);
    JSON_ADD_INT_FROM_SETTINGS(json_cfg, watermark);
    JSON_ADD_INT_FROM_SETTINGS(json_cfg, speclist);

    JSON_ADD_COL_FROM_SETTINGS(json_cfg, col_friend_esp);
    JSON_ADD_COL_FROM_SETTINGS(json_cfg, col_enemy_esp);
    JSON_ADD_COL_FROM_SETTINGS(json_cfg, col_friend_build);
    JSON_ADD_COL_FROM_SETTINGS(json_cfg, col_enemy_build);
    JSON_ADD_COL_FROM_SETTINGS(json_cfg, col_ammobox_esp);
    JSON_ADD_COL_FROM_SETTINGS(json_cfg, col_healthpack_esp);

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

#define LOAD_PRINT_ERROR()                                         \
    {                                                              \
        fprintf(stderr, "load_config: %s\n", cJSON_GetErrorPtr()); \
        free(json_cfg_str);                                        \
        cJSON_Delete(json_cfg);                                    \
        return;                                                    \
    }

#define LOAD_ABORT(...)               \
    {                                 \
        fprintf(stderr, __VA_ARGS__); \
        free(json_cfg_str);           \
        cJSON_Delete(json_cfg);       \
        return;                       \
    }

#define JSON_LOAD_INT_TO_SETTINGS(PARENT, ITEM)                             \
    cJSON* json_##ITEM = cJSON_GetObjectItemCaseSensitive(PARENT, #ITEM);   \
    if (!cJSON_IsNumber(json_##ITEM))                                       \
        LOAD_ABORT("load_config: setting \"%s\" is not a number\n", #ITEM); \
    settings.ITEM = json_##ITEM->valueint;

#define JSON_LOAD_COL_TO_SETTINGS(PARENT, ITEM)                                \
    cJSON* json_##ITEM = cJSON_GetObjectItemCaseSensitive(PARENT, #ITEM);      \
    if (!cJSON_IsObject(json_##ITEM))                                          \
        LOAD_ABORT("load_config: setting \"%s\" is not a rgba color object\n", \
                   #ITEM);                                                     \
    cJSON* json_##ITEM##_r =                                                   \
      cJSON_GetObjectItemCaseSensitive(json_##ITEM, "r");                      \
    if (!cJSON_IsNumber(json_##ITEM##_r))                                      \
        LOAD_ABORT("load_config: red field of setting \"%s\" is not a "        \
                   "number\n",                                                 \
                   #ITEM);                                                     \
    cJSON* json_##ITEM##_g =                                                   \
      cJSON_GetObjectItemCaseSensitive(json_##ITEM, "g");                      \
    if (!cJSON_IsNumber(json_##ITEM##_g))                                      \
        LOAD_ABORT("load_config: green field of setting \"%s\" is not a "      \
                   "number\n",                                                 \
                   #ITEM);                                                     \
    cJSON* json_##ITEM##_b =                                                   \
      cJSON_GetObjectItemCaseSensitive(json_##ITEM, "b");                      \
    if (!cJSON_IsNumber(json_##ITEM##_b))                                      \
        LOAD_ABORT("load_config: blue field of setting \"%s\" is not a "       \
                   "number\n",                                                 \
                   #ITEM);                                                     \
    cJSON* json_##ITEM##_a =                                                   \
      cJSON_GetObjectItemCaseSensitive(json_##ITEM, "a");                      \
    if (!cJSON_IsNumber(json_##ITEM##_a))                                      \
        LOAD_ABORT("load_config: alpha field of setting \"%s\" is not a "      \
                   "number\n",                                                 \
                   #ITEM);                                                     \
    settings.ITEM.r = json_##ITEM##_r->valuedouble;                            \
    settings.ITEM.g = json_##ITEM##_g->valuedouble;                            \
    settings.ITEM.b = json_##ITEM##_b->valuedouble;                            \
    settings.ITEM.a = json_##ITEM##_a->valuedouble;

void load_config(const char* filename) {
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

    JSON_LOAD_INT_TO_SETTINGS(json_cfg, player_esp);
    JSON_LOAD_INT_TO_SETTINGS(json_cfg, player_box_esp);
    JSON_LOAD_INT_TO_SETTINGS(json_cfg, skeleton_esp);
    JSON_LOAD_INT_TO_SETTINGS(json_cfg, player_health_esp);
    JSON_LOAD_INT_TO_SETTINGS(json_cfg, player_name_esp);
    JSON_LOAD_INT_TO_SETTINGS(json_cfg, player_class_esp);
    JSON_LOAD_INT_TO_SETTINGS(json_cfg, player_weapon_esp);
    JSON_LOAD_INT_TO_SETTINGS(json_cfg, building_esp);
    JSON_LOAD_INT_TO_SETTINGS(json_cfg, building_esp_type);
    JSON_LOAD_INT_TO_SETTINGS(json_cfg, building_box_esp);
    JSON_LOAD_INT_TO_SETTINGS(json_cfg, building_hp_esp);
    JSON_LOAD_INT_TO_SETTINGS(json_cfg, building_name_esp);
    JSON_LOAD_INT_TO_SETTINGS(json_cfg, ammobox_esp);
    JSON_LOAD_INT_TO_SETTINGS(json_cfg, healthpack_esp);

    JSON_LOAD_INT_TO_SETTINGS(json_cfg, bhop);
    JSON_LOAD_INT_TO_SETTINGS(json_cfg, autostrafe);
    JSON_LOAD_INT_TO_SETTINGS(json_cfg, autostab);
    JSON_LOAD_INT_TO_SETTINGS(json_cfg, watermark);
    JSON_LOAD_INT_TO_SETTINGS(json_cfg, speclist);

    JSON_LOAD_COL_TO_SETTINGS(json_cfg, col_friend_esp);
    JSON_LOAD_COL_TO_SETTINGS(json_cfg, col_enemy_esp);
    JSON_LOAD_COL_TO_SETTINGS(json_cfg, col_friend_build);
    JSON_LOAD_COL_TO_SETTINGS(json_cfg, col_enemy_build);
    JSON_LOAD_COL_TO_SETTINGS(json_cfg, col_ammobox_esp);
    JSON_LOAD_COL_TO_SETTINGS(json_cfg, col_healthpack_esp);

    free(json_cfg_str);
    cJSON_Delete(json_cfg);

    printf("[Enoch] Loaded config %s\n", filename);
}
