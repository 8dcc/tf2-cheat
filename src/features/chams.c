
#include "features.h"
#include "../include/sdk.h"
#include "../include/hooks.h"
#include "../include/globals.h"

/* I rather keep the menu simple, so change here what you need */
#define WPN_IGNOREZ    false
#define WPN_WIREFRAME  false
#define HAND_IGNOREZ   false
#define HAND_WIREFRAME false

/*----------------------------------------------------------------------------*/

static void override_material(bool ignorez, bool wireframe,
                              struct nk_colorf col, const char* mat_name) {
    IMaterial* material = METHOD_ARGS(i_materialsystem, FindMaterial, mat_name,
                                      TEXTURE_GROUP_MODEL, true, NULL);
    if (!material)
        return;

    /* Set renderview colors. Will be reset from h_DrawModelExecute later */
    METHOD_ARGS(i_renderview, SetColorModulation, (float_rgba_t*)&col);
    METHOD_ARGS(i_renderview, SetBlend, col.a);

    /* Set material flags */
    METHOD_ARGS(material, SetMaterialVarFlag, MATERIAL_VAR_IGNOREZ, ignorez);
    METHOD_ARGS(material, SetMaterialVarFlag, MATERIAL_VAR_WIREFRAME,
                wireframe);

    /* Override material with our own */
    METHOD_ARGS(i_modelrender, ForcedMaterialOverride, material,
                OVERRIDE_NORMAL);
}

/*----------------------------------------------------------------------------*/

void chams(ModelRender* thisptr, const DrawModelState_t* state,
           const ModelRenderInfo_t* pInfo, matrix3x4_t* pCustomBoneToWorld) {
    const model_t* mdl = pInfo->pModel;
    if (!mdl)
        return;

    if ((settings.chams_player != SETT_OFF || settings.chams_local) &&
        strstr(mdl->name, "models/player")) {
        if (pInfo->entity_index < 1 || pInfo->entity_index >= g.MaxClients)
            return;

        Entity* ent = g.ents[pInfo->entity_index];
        if (!g.localplayer || !ent)
            return;

        /* Localplayer chams (thirdperson) if enabled */
        if (g.localidx == pInfo->entity_index) {
            if (settings.chams_local)
                override_material(false, false, settings.col_chams_local,
                                  "debug/debugambientcube");

            return;
        }

        /* Next part is for non-local player chams */
        if (settings.chams_player == SETT_OFF)
            return;

        const bool teammate = IsTeammate(ent);

        /* Are chams enabled for this player's team? */
        if (settings.chams_player != SETT_ALL &&
            ((settings.chams_player == SETT_ENEMY && teammate) ||
             (settings.chams_player == SETT_FRIEND && !teammate)))
            return;

        struct nk_colorf vis_col = teammate ? settings.col_chams_friend
                                            : settings.col_chams_enemy;

        /* Invisible player chams */
        if (settings.chams_ignorez) {
            const rgba_t col                 = NK2COL(vis_col);
            const rgba_t scale               = col_scale(col, 0.50f);
            const struct nk_colorf invis_col = COL2NK(scale);

            override_material(true, false, invis_col, "debug/debugambientcube");
            ORIGINAL(DrawModelExecute, thisptr, state, pInfo,
                     pCustomBoneToWorld);
        }

        /* Visible player chams */
        override_material(false, false, vis_col, "debug/debugambientcube");
        return;
    }

    /* NOTE: We need to check arms and weapons in this order because the arms
     * model contains "weapons/c_" */
    if (strstr(mdl->name, "arms")) {
        if (!settings.chams_hand)
            return;

        override_material(HAND_IGNOREZ, HAND_WIREFRAME, settings.col_chams_hand,
                          "debug/debugambientcube");
        return;
    } else if (strstr(mdl->name, "weapons/c")) {
        if (!settings.chams_weapon)
            return;

#if 0
        /* TODO: Change weapon color depending on owner/only change viewmodel */
        Weapon* wpn = (Weapon*)g.ents[pInfo->entity_index];
        if (!wpn || !CBaseHandle_IsValid(wpn->hOwner))
            return;

        const int owner_idx = CBaseHandle_GetEntryIndex(wpn->hOwner);
        if (owner_idx < 1 || owner_idx >= g.MaxClients)
            return;

        Entity* owner = g.ents[owner_idx];
        if (!owner)
            return;
#endif

        override_material(WPN_IGNOREZ, WPN_WIREFRAME, settings.col_chams_weapon,
                          "debug/debugambientcube");
        return;
    }

    return;
}
