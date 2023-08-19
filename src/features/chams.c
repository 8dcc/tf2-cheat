
#include "features.h"
#include "../include/sdk.h"
#include "../include/globals.h"

/* I rather keep the menu simple, so change here what you need */
#define WPN_IGNOREZ    false
#define WPN_WIREFRAME  false
#define HAND_IGNOREZ   false
#define HAND_WIREFRAME false

/*----------------------------------------------------------------------------*/

#define CALL_ORIGINAL false
#define SKIP_ORIGINAL true

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

bool chams(const ModelRenderInfo_t* pInfo) {
    const model_t* mdl = pInfo->pModel;
    if (!mdl)
        return CALL_ORIGINAL;

    if (settings.player_chams != OFF && strstr(mdl->name, "models/player")) {
        Entity* ent = g.ents[pInfo->entity_index];
        if (!g.localplayer || !ent)
            return CALL_ORIGINAL;

        /* TODO: Localplayer chams after thirdperson */
        if (g.localplayer == ent)
            return CALL_ORIGINAL;

        const bool teammate = IsTeammate(ent);

        /* Are chams enabled for this player's team? */
        if (settings.player_chams == OFF ||
            (settings.player_chams == ENEMY && teammate) ||
            (settings.player_chams == FRIENDLY && !teammate))
            return CALL_ORIGINAL;

        struct nk_colorf vis_col   = teammate ? settings.col_friend_chams
                                              : settings.col_enemy_chams;
        struct nk_colorf invis_col = COL2NK(col_scale(NK2COL(vis_col), 0.70f));

        /* TODO: Only visible chams setting */
        if (!true) {
            /* TODO: Override and call original directly */
        }

        override_material(HAND_IGNOREZ, HAND_WIREFRAME, vis_col,
                          "debug/debugambientcube");
        return CALL_ORIGINAL;
    }

    /* NOTE: We need to check arms and weapons in this order because the arms
     * model contains "weapons/c_" */
    if (strstr(mdl->name, "arms")) {
        if (!settings.hand_chams)
            return CALL_ORIGINAL;

        override_material(HAND_IGNOREZ, HAND_WIREFRAME, settings.col_hand_chams,
                          "debug/debugambientcube");
        return CALL_ORIGINAL;
    } else if (strstr(mdl->name, "weapons/c")) {
        if (!settings.weapon_chams)
            return CALL_ORIGINAL;

#if 0
        /* TODO: Change weapon color depending on owner/only change viewmodel */
        Weapon* wpn = (Weapon*)g.ents[pInfo->entity_index];
        if (!wpn || !CBaseHandle_IsValid(wpn->hOwner))
            return CALL_ORIGINAL;

        const int owner_idx = CBaseHandle_GetEntryIndex(wpn->hOwner);
        if (owner_idx < 1 || owner_idx >= g.MaxClients)
            return CALL_ORIGINAL;

        Entity* owner = g.ents[owner_idx];
        if (!owner)
            return CALL_ORIGINAL;
#endif

        override_material(WPN_IGNOREZ, WPN_WIREFRAME, settings.col_weapon_chams,
                          "debug/debugambientcube");
        return CALL_ORIGINAL;
    }

    return CALL_ORIGINAL;
}
