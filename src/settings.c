
#include <stdbool.h>
#include "include/settings.h"

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
    .building_box_esp  = false,
    .building_hp_esp   = false,
    .building_type_esp = false,
    .ammobox_esp       = false,
    .healthpack_esp    = false,

    /* Movement */
    .bhop       = false,
    .autostrafe = OFF,

    /* Misc */
    .watermark = true,
    .speclist  = true,
    .autostab  = false,

    /* Colors */
    .col_friend_esp     = (struct nk_colorf){ 0.05f, 0.47f, 0.95f, 1.f },
    .col_enemy_esp      = (struct nk_colorf){ 0.95f, 0.10f, 0.09f, 1.f },
    .col_friend_build   = (struct nk_colorf){ 0.29f, 0.07f, 0.54f, 1.f },
    .col_enemy_build    = (struct nk_colorf){ 0.90f, 0.31f, 0.00f, 1.f },
    .col_ammobox_esp    = (struct nk_colorf){ 0.55f, 0.43f, 0.38f, 1.f },
    .col_healthpack_esp = (struct nk_colorf){ 0.40f, 0.73f, 0.41f, 1.f },
};

/*----------------------------------------------------------------------------*/

/* TODO: Json setting system */
