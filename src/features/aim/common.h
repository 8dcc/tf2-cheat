
#include "../../include/sdk.h"
#include "../../include/settings.h"

static inline void setting_to_hitboxes(int setting, int* min, int* max) {
    switch (setting) {
        case SETT_HITBOX_HEAD:
            *min = HITBOX_HEAD;
            *max = HITBOX_HEAD;
            break;
        case SETT_HITBOX_TORSO:
            *min = HITBOX_PELVIS;
            *max = HITBOX_SPINE3;
            break;
        case SETT_HITBOX_ARMS:
            *min = HITBOX_LEFT_UPPER_ARM;
            *max = HITBOX_RIGHT_HAND;
            break;
        case SETT_HITBOX_LEGS:
            *min = HITBOX_LEFT_HIP;
            *max = HITBOX_RIGHT_FOOT;
            break;
    }
}

#define HITBOX_SET 0
static vec3_t get_hitbox_pos(Entity* ent, int hitbox_idx) {
    static matrix3x4_t bones[MAXSTUDIOBONES];

    Renderable* rend = GetRenderable(ent);

    if (!METHOD_ARGS(rend, SetupBones, bones, MAXSTUDIOBONES,
                     BONE_USED_BY_HITBOX, 0))
        return VEC_ZERO;

    const model_t* model = METHOD(rend, GetModel);
    if (!model)
        return VEC_ZERO;

    studiohdr_t* hdr = METHOD_ARGS(i_modelinfo, GetStudioModel, model);
    if (!hdr)
        return VEC_ZERO;

    return center_of_hitbox(hdr, bones, HITBOX_SET, hitbox_idx);
}
