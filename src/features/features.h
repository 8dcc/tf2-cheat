#ifndef FEATURES_H_
#define FEATURES_H_

#include "../include/sdk.h"
#include "../include/settings.h"

/* src/features/esp.c */
void esp(void);

/* src/features/chams.c */
bool chams(ModelRender* thisptr, const DrawModelState_t* state,
           const ModelRenderInfo_t* pInfo, matrix3x4_t* pCustomBoneToWorld);

/* src/features/aim/aimbot.c */
void aimbot(usercmd_t* cmd);
void draw_aim_fov(void);

/* src/features/aim/meleebot.c */
void meleebot(usercmd_t* cmd);

/* src/features/movement.c */
void bhop(usercmd_t* cmd);
void autorocketjump(usercmd_t* cmd);
void correct_movement(usercmd_t* cmd, vec3_t old_angles);

/* src/features/misc.c */
void autobackstab(usercmd_t* cmd);
void spectator_list(void);
void thirdperson(void);

/* src/features/prediction.c */
void pred_start(usercmd_t* cmd);
void pred_end(void);

#endif /* FEATURES_H_ */
