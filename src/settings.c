
#include <stdbool.h>
#include "include/settings.h"

/* Global settings */
Settings settings = {
    .bhop       = false,
    .autostrafe = OFF,

    .enable_esp     = OFF,
    .box_esp        = false,
    .health_esp     = false,
    .name_esp       = false,
    .class_esp      = false,
    .weapon_esp     = false,
    .col_friend_esp = (struct nk_colorf){ 0.05f, 0.47f, 0.95f, 1.f },
    .col_enemy_esp  = (struct nk_colorf){ 0.95f, 0.10f, 0.09f, 1.f },
    .col_ammo_esp   = (struct nk_colorf){ 0.55f, 0.43f, 0.38f, 1.f },
    .col_health_esp = (struct nk_colorf){ 0.40f, 0.73f, 0.41f, 1.f },

    .watermark = true,
    .speclist  = true,
    .autostab  = false,
};

/*----------------------------------------------------------------------------*/

/* TODO: Json setting system */
