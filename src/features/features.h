#ifndef FEATURES_H_
#define FEATURES_H_

#include "../include/sdk.h"
#include "../include/settings.h"

/* src/features/esp.c */
void esp(void);

/* src/features/aim.c */
void aimbot(usercmd_t* cmd);

/* src/features/movement.c */
void bhop(usercmd_t* cmd);
void correct_movement(usercmd_t* cmd, vec3_t old_angles);

/* src/features/misc.c */
void autobackstab(usercmd_t* cmd);

#endif /* FEATURES_H_ */
