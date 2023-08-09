#ifndef FEATURES_H_
#define FEATURES_H_

#include "../include/sdk.h"
#include "../include/settings.h"

/* src/features/movement.c */
void bhop(usercmd_t* cmd);

/* src/features/esp.c */
void esp(void);

/* src/features/misc.c */
void autobackstab(usercmd_t* cmd);

#endif /* FEATURES_H_ */
