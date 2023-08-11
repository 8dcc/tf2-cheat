
#include <stdbool.h>
#include "include/settings.h"

/* Global settings */
Settings settings = {
    .bhop       = false,
    .autostrafe = OFF,

    .enable_esp = OFF,
    .box_esp    = false,
    .name_esp   = false,
    .class_esp  = false,
    .weapon_esp = false,

    .watermark = true,
    .speclist  = true,
    .autostab  = false,
};

/*----------------------------------------------------------------------------*/

/* TODO: Json setting system */
