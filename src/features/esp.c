
#include "features.h"
#include "../include/sdk.h"
#include "../include/globals.h"

#define INFOPOS_LINE_H 11    /* Spacing between ESP lines */
#define COL_SCALE      1.25f /* Color scale per ESP text item */

#define OUTLINED_BOX(x, y, w, h, c)                                           \
    {                                                                         \
        METHOD_ARGS(i_surface, SetColor, 0, 0, 0, c.a);                       \
        METHOD_ARGS(i_surface, DrawRect, x - 1, y - 1, x + w + 1, y + h + 1); \
        METHOD_ARGS(i_surface, DrawRect, x + 1, y + 1, x + w - 1, y + h - 1); \
        METHOD_ARGS(i_surface, SetColor, c.r, c.g, c.b, c.a);                 \
        METHOD_ARGS(i_surface, DrawRect, x, y, x + w, y + h);                 \
    }

static inline bool get_bbox(Entity* ent, int* x, int* y, int* w, int* h) {
    Collideable* collideable = METHOD(ent, GetCollideable);
    if (!collideable)
        return false;

    vec3_t obb_mins        = *METHOD(collideable, ObbMins);
    vec3_t obb_maxs        = *METHOD(collideable, ObbMaxs);
    Renderable* renderable = GetRenderable(ent);

    matrix3x4_t* trans = METHOD(renderable, RenderableToWorldTransform);
    if (!trans)
        return false;

    vec3_t points[] = { { obb_mins.x, obb_mins.y, obb_mins.z },
                        { obb_mins.x, obb_maxs.y, obb_mins.z },
                        { obb_maxs.x, obb_maxs.y, obb_mins.z },
                        { obb_maxs.x, obb_mins.y, obb_mins.z },
                        { obb_maxs.x, obb_maxs.y, obb_maxs.z },
                        { obb_mins.x, obb_maxs.y, obb_maxs.z },
                        { obb_mins.x, obb_mins.y, obb_maxs.z },
                        { obb_maxs.x, obb_mins.y, obb_maxs.z } };

    for (int i = 0; i < 8; i++) {
        vec3_t t;
        vec_transform(points[i], trans, &t);

        vec2_t s;
        if (!world_to_screen(t, &s))
            return false;

        points[i].x = s.x;
        points[i].y = s.y;
    }

    float left   = points[0].x;
    float bottom = points[0].y;
    float right  = points[0].x;
    float top    = points[0].y;

    for (int i = 0; i < 8; i++) {
        if (left > points[i].x)
            left = points[i].x;
        if (bottom < points[i].y)
            bottom = points[i].y;
        if (right < points[i].x)
            right = points[i].x;
        if (top > points[i].y)
            top = points[i].y;
    }

    *x = (int)(left);
    *y = (int)(top);
    *w = (int)(right - left);
    *h = (int)(bottom - top);

    return true;
}

static inline void skeleton_esp(Renderable* rend, matrix3x4_t* bones,
                                rgba_t col) {
    if (!settings.skeleton_esp)
        return;

    if (!METHOD_ARGS(rend, SetupBones, bones, MAXSTUDIOBONES,
                     BONE_USED_BY_HITBOX, 0))
        return;

    const model_t* model = METHOD(rend, GetModel);
    if (!model)
        return;

    studiohdr_t* hdr = METHOD_ARGS(i_modelinfo, GetStudioModel, model);
    if (!hdr)
        return;

    for (int i = 0; i < hdr->numbones; i++) {
        studiobone_t* bone = studiohdr_pBone(hdr, i);
        if (!bone || bone->parent < 0 || bone->parent >= MAXSTUDIOBONES ||
            !(bone->flags & BONE_USED_BY_HITBOX))
            continue;

        /* bones ==> (matrix3x4_t)[MAXSTUDIOBONES].m[3][4] */
        const vec3_t child = (vec3_t){
            bones[i].m[0][3],
            bones[i].m[1][3],
            bones[i].m[2][3],
        };

        const vec3_t parent = (vec3_t){
            bones[bone->parent].m[0][3],
            bones[bone->parent].m[1][3],
            bones[bone->parent].m[2][3],
        };

        vec2_t s_child, s_parent;
        if (!world_to_screen(child, &s_child) ||
            !world_to_screen(parent, &s_parent))
            continue;

        METHOD_ARGS(i_surface, SetColor, col.r, col.g, col.b, col.a);
        METHOD_ARGS(i_surface, DrawLine, s_child.x, s_child.y, s_parent.x,
                    s_parent.y);
    }
}

#define GENERIC_ENT_NAME(ent, name, col) \
    if (!get_bbox(ent, &x, &y, &w, &h))  \
        continue;                        \
    draw_text(x + w / 2, y + w / 2, true, g_fonts.small.id, col, name);

void esp(void) {
    if (settings.player_esp == OFF && !settings.ammobox_esp &&
        !settings.healthpack_esp)
        return;

    if (!g.localplayer || !g.IsConnected)
        return;

    rgba_t player_friend_col = NK2COL(settings.col_friend_esp);
    rgba_t player_enemy_col  = NK2COL(settings.col_enemy_esp);
    rgba_t ammobox_col       = NK2COL(settings.col_ammobox_esp);
    rgba_t healthpack_col    = NK2COL(settings.col_healthpack_esp);

    /* For bounding box */
    int x, y, w, h;

    /* For skeleton ESP */
    static matrix3x4_t bones[MAXSTUDIOBONES];

    /* Iterate all entities */
    for (int i = 1; i < g.MaxEntities; i++) {
        if (i == g.localidx)
            continue;

        Entity* ent      = g.ents[i];
        Networkable* net = GetNetworkable(ent);
        Renderable* rend = GetRenderable(ent);
        if (!ent)
            continue;

        ClientClass* ent_class = METHOD(net, GetClientClass);
        if (!ent_class)
            continue;

        switch (ent_class->class_id) {
            case CClass_CTFPlayer: {
                const bool teammate = IsTeammate(ent);

                /* Should we render this player's team? */
                switch (settings.player_esp) {
                    case ENEMY:
                        if (teammate)
                            continue;
                        break;
                    case FRIENDLY:
                        if (!teammate)
                            continue;
                        break;
                    case ALL:
                        break;
                    default:
                    case OFF:
                        continue;
                }

                if (!get_bbox(ent, &x, &y, &w, &h))
                    continue;

                rgba_t col = teammate ? player_friend_col : player_enemy_col;

                /*------------------------------------------------------------*/
                /* Player skeleton ESP */

                skeleton_esp(rend, bones, col);

                /*------------------------------------------------------------*/
                /* Player box ESP */

                if (settings.box_esp)
                    OUTLINED_BOX(x, y, w, h, col);

                /*------------------------------------------------------------*/
                /* Player health ESP */

                if (settings.player_health_esp) {
                    const int hp     = METHOD(ent, GetHealth);
                    const int max_hp = METHOD(ent, GetMaxHealth);

                    const int hpx = x - 5;
                    const int hpy = y;
                    const int hpw = 2;
                    const int hph = h;

                    /* Background (red) */
                    METHOD_ARGS(i_surface, SetColor, 0, 0, 0, col.a);
                    METHOD_ARGS(i_surface, DrawRect, hpx - 1, hpy - 1,
                                hpx + hpw + 1, hpy + h + 1);
                    METHOD_ARGS(i_surface, DrawRect, hpx + 1, hpy + 1,
                                hpx + hpw - 1, hpy + h - 1);
                    METHOD_ARGS(i_surface, SetColor, 170, 29, 29, col.a);
                    METHOD_ARGS(i_surface, DrawFilledRect, hpx, hpy, hpx + hpw,
                                hpy + hph);

                    /* Health bar (green) */
                    const int hpbar_h = (hph * MIN(hp, max_hp) / max_hp);
                    const int hpbar_y = hpy + (hph - hpbar_h);
                    METHOD_ARGS(i_surface, SetColor, 67, 160, 71, col.a);
                    METHOD_ARGS(i_surface, DrawFilledRect, hpx, hpbar_y,
                                hpx + hpw, hpbar_y + hpbar_h);

                    /* Health text */
                    const int hpbar_x = (settings.box_esp) ? hpx - 2 : hpx;
                    static char hp_txt[5];
                    sprintf(hp_txt, "%d", hp);
                    draw_text(hpbar_x, hpy - 13, !settings.box_esp,
                              g_fonts.small.id, (rgba_t){ 34, 193, 41, col.a },
                              hp_txt);
                }

                /*------------------------------------------------------------*/
                /* Player name ESP */

                int infopos_x = x + w / 2;
                int infopos_y = y + h + 2;

                if (settings.name_esp) {
                    player_info_t pinfo;
                    METHOD_ARGS(i_engine, GetPlayerInfo, i, &pinfo);

                    draw_text(infopos_x, infopos_y, true, g_fonts.main.id, col,
                              pinfo.name);

                    col = col_scale(col, COL_SCALE);
                    infopos_y += INFOPOS_LINE_H;
                }

                /*------------------------------------------------------------*/
                /* Player class ESP */

                if (settings.class_esp) {
                    draw_text(infopos_x, infopos_y, true, g_fonts.main.id, col,
                              GetClassName(ent));

                    col = col_scale(col, COL_SCALE);
                    infopos_y += INFOPOS_LINE_H;
                }

                /*------------------------------------------------------------*/
                /* Player weapon ESP */

                if (settings.weapon_esp) {
                    Weapon* weapon = METHOD(ent, GetWeapon);

                    if (weapon) {
                        const char* wname = METHOD(weapon, GetName);
                        if (!strncmp(wname, "tf_weapon_", 10))
                            wname += 10;

                        draw_text(infopos_x, infopos_y, true, g_fonts.main.id,
                                  col, wname);

                        col = col_scale(col, COL_SCALE);
                        infopos_y += INFOPOS_LINE_H;
                    }
                }

                /* end: case CClass_CTFPlayer */
                break;
            }

            case CClass_CTFAmmoPack:
                /* Dropped ammo, not the normal ammo boxes */
                if (settings.ammobox_esp) {
                    GENERIC_ENT_NAME(ent, "Ammo", ammobox_col);
                }
                break;

            case CClass_CCurrencyPack:
                /* I am not adding another option/color for this shit */
                if (settings.ammobox_esp) {
                    GENERIC_ENT_NAME(ent, "Currency", ammobox_col);
                }
                break;

            case CClass_CBaseAnimating: {
                bool drawn = false;

                if (!drawn && settings.healthpack_esp) {
                    /* Check that this model is in the healthpack range */
                    for (int j = MDLIDX_MEDKIT_SMALL;
                         j <= MDLIDX_MUSHROOM_LARGE; j++) {
                        if (ent->model_idx == g.mdl_idx[j]) {
                            GENERIC_ENT_NAME(ent, "Health", healthpack_col);
                            drawn = true;
                            break;
                        }
                    }
                }

                if (!drawn && settings.ammobox_esp) {
                    /* Check that this model is in the healthpack range */
                    for (int j = MDLIDX_AMMOPACK_SMALL;
                         j <= MDLIDX_AMMOPACK_SMALL_BDAY; j++) {
                        if (ent->model_idx == g.mdl_idx[j]) {
                            GENERIC_ENT_NAME(ent, "Ammo", ammobox_col);
                            drawn = true;
                            break;
                        }
                    }
                }

                /* end: case CClass_CBaseAnimating */
                break;
            }

            default:
                break;
        } /* end: Entity class switch */
    }     /* end: Entity for loop */
} /* end: esp() */
