#ifndef FEATURES_H_
#define FEATURES_H_

#include "../include/sdk.h"
#include "../include/settings.h"

/* src/features/esp.c */
void esp(void);

/* src/features/chams.c */
void chams(ModelRender* thisptr, const DrawModelState_t* state,
           const ModelRenderInfo_t* pInfo, matrix3x4_t* pCustomBoneToWorld);

/* src/features/aim/aimbot.c */
extern bool aimbot_key_down;
void aimbot(usercmd_t* cmd);
void draw_aim_fov(void);

/* src/features/aim/meleebot.c */
extern bool meleebot_key_down;
void meleebot(usercmd_t* cmd);

/* src/features/aim/medigun.c */
void automedigun(usercmd_t* cmd);

/* src/features/antiaim.c */
void spinbot(usercmd_t* cmd);

/* src/features/auto_detonate_stickies.c */
void auto_detonate_stickies(usercmd_t* cmd);

/* src/features/movement.c */
void bhop(usercmd_t* cmd);
void autorocketjump(usercmd_t* cmd);
void correct_movement(usercmd_t* cmd, vec3_t old_angles);

/* src/features/visuals.c */
void spectator_list(void);
void store_velocity(void);
void draw_velocity(void);
void thirdperson(void);
void custom_fov(ViewSetup* pSetup);

/* src/features/misc.c */
void autobackstab(usercmd_t* cmd);
void nopush(void);
void antiafk(usercmd_t* cmd);

/* src/features/prediction.c */
void pred_start(usercmd_t* cmd);
void pred_end(void);

/* src/features/playerlist.c */
void playerlist_full_update();
void playerlist_print_all();

#endif /* FEATURES_H_ */
