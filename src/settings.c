
#include <stdbool.h>
#include "include/settings.h"

/* Global settings */
Settings settings = {
    .bhop       = false,
    .autostrafe = OFF,

    .enable_esp     = OFF,
    .col_friend_esp = (struct nk_colorf){ 0.05f, 0.47f, 0.95f, 1.f },
    .col_enemy_esp  = (struct nk_colorf){ 0.95f, 0.10f, 0.09f, 1.f },
    .box_esp        = false,
    .health_esp     = false,
    .name_esp       = false,
    .class_esp      = false,
    .weapon_esp     = false,

    .watermark = true,
    .speclist  = true,
    .autostab  = false,
};

/*----------------------------------------------------------------------------*/

/* TODO: Json setting system */
