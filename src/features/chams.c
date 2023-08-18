
#include "features.h"
#include "../include/sdk.h"
#include "../include/globals.h"

#define CALL_ORIGINAL false
#define SKIP_ORIGINAL true

bool chams(const DrawModelState_t* state, const ModelRenderInfo_t* pInfo,
           matrix3x4_t* pCustomBoneToWorld) {
    const model_t* mdl = pInfo->pModel;
    if (!mdl)
        return CALL_ORIGINAL;

    if (strstr(mdl->name, "arms"))
        return SKIP_ORIGINAL;

    return CALL_ORIGINAL;
}
