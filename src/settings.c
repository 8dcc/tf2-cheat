
#include <stdbool.h>
#include "include/settings.h"

/* Global settings */
Settings settings = {
    .bhop       = false,
    .autostrafe = OFF,

    .enable_esp = OFF,
    .box_esp    = false,
    .name_esp   = false,
    .weapon_esp = false,

    .autostab   = false,
    .slide_walk = false,
};

/*----------------------------------------------------------------------------*/

/* TODO: Json setting system */
