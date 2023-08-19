
#include "features.h"
#include "../include/sdk.h"
#include "../include/globals.h"

#define CALL_ORIGINAL false
#define SKIP_ORIGINAL true

static void override_material(bool ignorez, bool wireframe, rgba_t col,
                              const char* mat_name) {
    IMaterial* material = METHOD_ARGS(i_materialsystem, FindMaterial, mat_name,
                                      TEXTURE_GROUP_MODEL, true, NULL);
    if (!material)
        return;

    METHOD_ARGS(i_renderview, SetBlend, 1.f);

    METHOD_ARGS(material, SetMaterialVarFlag, MATERIAL_VAR_IGNOREZ, ignorez);
    METHOD_ARGS(material, SetMaterialVarFlag, MATERIAL_VAR_WIREFRAME,
                wireframe);

    METHOD_ARGS(material, AlphaModulate, col.a / 255.f);
    METHOD_ARGS(material, ColorModulate, col.r / 255.f, col.g / 255.f,
                col.b / 255.f);

    METHOD_ARGS(i_modelrender, ForcedMaterialOverride, material,
                OVERRIDE_NORMAL);
}

bool chams(const ModelRenderInfo_t* pInfo) {
    const model_t* mdl = pInfo->pModel;
    if (!mdl)
        return CALL_ORIGINAL;

    if (strstr(mdl->name, "arms"))
        override_material(false, false, (rgba_t){ 225, 190, 231, 100 },
                          "debug/debugambientcube");

    return CALL_ORIGINAL;
}
