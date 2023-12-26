
#include "features.h"
#include "../include/sdk.h"
#include "../include/math.h"
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

/*----------------------------------------------------------------------------*/

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
    if (!settings.esp_skeleton)
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

static inline void building_esp(Entity* ent, const char* str, rgba_t friend_col,
                                rgba_t enemy_col) {
    const int our_teamnum  = METHOD(g.localplayer, GetTeamNumber);
    const int ent_teamnum  = METHOD(ent, GetTeamNumber);
    const bool is_teammate = (our_teamnum == ent_teamnum);

    /* Should we render this building's team? */
    if (settings.esp_building != SETT_ALL &&
        ((settings.esp_building == SETT_FRIEND && !is_teammate) ||
         (settings.esp_building == SETT_ENEMY && is_teammate)))
        return;

    static int x, y, w, h;
    if (!get_bbox(ent, &x, &y, &w, &h))
        return;

    rgba_t col = settings.esp_building_use_team_color
                   ? NK2COL(get_team_color(ent_teamnum))
                 : is_teammate ? friend_col
                               : enemy_col;

    /* Building box ESP */
    if (settings.esp_building_box)
        OUTLINED_BOX(x, y, w, h, col);

    /* Building health ESP */
    if (settings.esp_building_hp) {
        const int hp     = METHOD(ent, GetHealth);
        const int max_hp = METHOD(ent, GetMaxHealth);

        const int hpx = x;
        const int hpy = y + h + 3;
        const int hpw = w;
        const int hph = 2;

        /* Background (red) */
        rgba_t redhp = (rgba_t){ 170, 29, 29, col.a };
        OUTLINED_BOX(hpx, hpy, hpw, hph, redhp);

        /* Health bar (green) */
        const int hpbar_w = (hpw * MIN(hp, max_hp) / max_hp);
        METHOD_ARGS(i_surface, SetColor, 67, 160, 71, col.a);
        METHOD_ARGS(i_surface, DrawFilledRect, hpx, hpy, hpx + hpbar_w,
                    hpy + hph);

        /* Health text */
        static char hp_txt[5];
        sprintf(hp_txt, "%d", hp);

        int textw, texth;
        get_text_size(g_fonts.small.id, hp_txt, &textw, &texth);

        draw_text(hpx - textw - 4, hpy - texth / 2 + 2, false, g_fonts.small.id,
                  (rgba_t){ 34, 193, 41, col.a }, hp_txt);

        y += 6;
    }

    /* Building type ESP (string function parameter passed from esp) */
    if (settings.esp_building_name)
        draw_text(x + w / 2, y + h + 1, true, g_fonts.main.id, col, str);
}

static inline void generic_ent_name(Entity* ent, const char* str, rgba_t col) {
    vec3_t center = GetCenter(ent);

    vec2_t screen;
    if (!world_to_screen(center, &screen))
        return;

    /* Subtract half the font height to center vertically as well */
    const int y_offset = g_fonts.small.tall / 2;
    draw_text(screen.x, screen.y - y_offset, true, g_fonts.small.id, col, str);
}

/*----------------------------------------------------------------------------*/

void esp(void) {
    if (settings.esp_player == SETT_OFF && settings.esp_building == SETT_OFF &&
        settings.esp_sticky == SETT_OFF && !settings.esp_ammobox &&
        !settings.esp_healthpack)
        return;

    if (!g.localplayer)
        return;

    rgba_t col_player_steam_friend = NK2COL(settings.col_esp_steam_friend);
    rgba_t col_player_friend       = NK2COL(settings.col_esp_friend);
    rgba_t col_player_enemy        = NK2COL(settings.col_esp_enemy);
    rgba_t col_build_friend        = NK2COL(settings.col_esp_friend_build);
    rgba_t col_build_enemy         = NK2COL(settings.col_esp_enemy_build);
    rgba_t col_sticky_friend       = NK2COL(settings.col_esp_sticky_friend);
    rgba_t col_sticky_enemy        = NK2COL(settings.col_esp_sticky_enemy);
    rgba_t col_ammobox             = NK2COL(settings.col_esp_ammobox);
    rgba_t col_healthpack          = NK2COL(settings.col_esp_healthpack);

    /* For bounding box */
    static int x, y, w, h;

    /* For skeleton ESP */
    static matrix3x4_t bones[MAXSTUDIOBONES];

    /* If we are dead, get the spectated player so we know his team, and we
     * don't render his ESP. */
    Entity* local_or_spectated = NULL;
    if (!g.IsAlive) {
        const int obs_mode = METHOD(g.localplayer, GetObserverMode);

        /* We are spectating in 1st or 3rd person */
        if ((obs_mode == OBS_MODE_IN_EYE || obs_mode == OBS_MODE_CHASE))
            local_or_spectated = METHOD(g.localplayer, GetObserverTarget);
    }

    /* We are not spectating a valid player, use ourselves */
    if (local_or_spectated == NULL)
        local_or_spectated = g.localplayer;

    /* Calculate once */
    const int local_idx   = METHOD(local_or_spectated, GetIndex);
    const int our_teamnum = METHOD(local_or_spectated, GetTeamNumber);

    /* Iterate all entities */
    for (int i = 1; i < g.MaxEntities; i++) {
        if (i == local_idx)
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
                /* Don't render ESP for the spectated player */
                if (settings.esp_player == SETT_OFF)
                    continue;

                const int ent_teamnum  = METHOD(ent, GetTeamNumber);
                const bool is_teammate = (our_teamnum == ent_teamnum);

                /* We don't want to render this team */
                if (settings.esp_player != SETT_ALL &&
                    ((settings.esp_player == SETT_FRIEND && !is_teammate) ||
                     (settings.esp_player == SETT_ENEMY && is_teammate)))
                    continue;

                /* We don't want to render invisible enemy spies */
                if (settings.esp_ignore_invisible && !is_teammate &&
                    IsInvisible(ent))
                    continue;

                if (!get_bbox(ent, &x, &y, &w, &h))
                    continue;

                rgba_t col = IsSteamFriend(ent) ? col_player_steam_friend
                             : settings.esp_use_team_color
                               ? NK2COL(get_team_color(ent_teamnum))
                             : is_teammate ? col_player_friend
                                           : col_player_enemy;

                /*------------------------------------------------------------*/
                /* Player skeleton ESP */

                skeleton_esp(rend, bones, col);

                /*------------------------------------------------------------*/
                /* Player box ESP */

                if (settings.esp_player_box)
                    OUTLINED_BOX(x, y, w, h, col);

                /*------------------------------------------------------------*/
                /* Player health ESP */

                if (settings.esp_player_health) {
                    const int hp     = METHOD(ent, GetHealth);
                    const int max_hp = METHOD(ent, GetMaxHealth);

                    const int hpx = x - 5;
                    const int hpy = y;
                    const int hpw = 2;
                    const int hph = h;

                    /* Background (red) */
                    rgba_t redhp = (rgba_t){ 170, 29, 29, col.a };
                    OUTLINED_BOX(hpx, hpy, hpw, hph, redhp);

                    /* Health bar (green) */
                    const int hpbar_h = (hph * MIN(hp, max_hp) / max_hp);
                    const int hpbar_y = hpy + (hph - hpbar_h);
                    METHOD_ARGS(i_surface, SetColor, 67, 160, 71, col.a);
                    METHOD_ARGS(i_surface, DrawFilledRect, hpx, hpbar_y,
                                hpx + hpw, hpbar_y + hpbar_h);

                    /* Health text */
                    const int hpbar_x = (settings.esp_player_box) ? hpx - 2
                                                                  : hpx;
                    static char hp_txt[5];
                    sprintf(hp_txt, "%d", hp);
                    draw_text(hpbar_x, hpy - 13, !settings.esp_player_box,
                              g_fonts.small.id, (rgba_t){ 34, 193, 41, col.a },
                              hp_txt);
                }

                /*------------------------------------------------------------*/
                /* Player name ESP */

                int infopos_x   = x + w / 2;
                int infopos_y   = y + h + 2;
                rgba_t info_col = col;

                if (settings.esp_player_name) {
                    player_info_t pinfo;
                    METHOD_ARGS(i_engine, GetPlayerInfo, i, &pinfo);

                    /* Replace multibyte chars with '?' */
                    static char converted_name[MAX_PLAYER_NAME_LENGTH];
                    convert_player_name(converted_name, pinfo.name);

                    draw_text(infopos_x, infopos_y, true, g_fonts.main.id,
                              info_col, converted_name);

                    info_col = col_scale(info_col, COL_SCALE);
                    infopos_y += INFOPOS_LINE_H;
                }

                /*------------------------------------------------------------*/
                /* Player class ESP */

                if (settings.esp_player_class) {
                    draw_text(infopos_x, infopos_y, true, g_fonts.main.id,
                              info_col, GetClassName(ent));

                    info_col = col_scale(info_col, COL_SCALE);
                    infopos_y += INFOPOS_LINE_H;
                }

                /*------------------------------------------------------------*/
                /* Player weapon ESP */

                if (settings.esp_player_weapon) {
                    Weapon* weapon = METHOD(ent, GetWeapon);

                    if (weapon) {
                        const char* wep_name = METHOD(weapon, GetName);
                        if (!strncmp(wep_name, "tf_weapon_", 10))
                            wep_name += 10;

                        draw_text(infopos_x, infopos_y, true, g_fonts.main.id,
                                  info_col, wep_name);

                        info_col = col_scale(info_col, COL_SCALE);
                        infopos_y += INFOPOS_LINE_H;
                    }
                }

                /*------------------------------------------------------------*/
                /* Cond ESP */

                if (settings.esp_player_cond) {
                    int cond_x = x + w + 2;
                    int cond_y = y + 1;
                    int cond_h = 12;

                    rgba_t cond_col = NK2COL(settings.col_esp_player_cond);
                    const HFont cond_font = g_fonts.small.id;

                    /* TODO: Uber ready? */

                    if (IsInvulnerable(ent)) {
                        draw_text(cond_x, cond_y, false, cond_font, cond_col,
                                  "Invul.");
                        cond_y += cond_h;
                    }

                    if (IsInvisible(ent)) {
                        draw_text(cond_x, cond_y, false, cond_font, cond_col,
                                  "Invis.");
                        cond_y += cond_h;
                    }

                    if (InCond(ent, TF_COND_DISGUISED)) {
                        draw_text(cond_x, cond_y, false, cond_font, cond_col,
                                  "Disguised");
                        cond_y += cond_h;
                    }

                    if (InCond(ent, TF_COND_TAUNTING)) {
                        draw_text(cond_x, cond_y, false, cond_font, cond_col,
                                  "Taunting");
                        cond_y += cond_h;
                    }

                    if (IsCritBoosted(ent)) {
                        draw_text(cond_x, cond_y, false, cond_font, cond_col,
                                  "Crit");
                        cond_y += cond_h;
                    }

                    if (IsMiniCritBoosted(ent)) {
                        draw_text(cond_x, cond_y, false, cond_font, cond_col,
                                  "Mini-Crit");
                        cond_y += cond_h;
                    }

                    if (InCond(ent, TF_COND_ZOOMED)) {
                        draw_text(cond_x, cond_y, false, cond_font, cond_col,
                                  "Scoped");
                        cond_y += cond_h;
                    }

                    if (IsMarked(ent)) {
                        draw_text(cond_x, cond_y, false, cond_font, cond_col,
                                  "Marked");
                        cond_y += cond_h;
                    }

                    if (InCond(ent, TF_COND_MAD_MILK)) {
                        draw_text(cond_x, cond_y, false, cond_font, cond_col,
                                  "Mad Milk");
                        cond_y += cond_h;
                    }

                    if (InCond(ent, TF_COND_BURNING)) {
                        draw_text(cond_x, cond_y, false, cond_font, cond_col,
                                  "Burning");
                        cond_y += cond_h;
                    }

                    if (InCond(ent, TF_COND_BLEEDING)) {
                        draw_text(cond_x, cond_y, false, cond_font, cond_col,
                                  "Bleeding");
                        cond_y += cond_h;
                    }
                }

                /* end: case CClass_CTFPlayer */
                break;
            }

            /* TODO: We could mark buildings when TF_COND_SAPPED */
            case CClass_CObjectSentrygun: {
                if (settings.esp_building_type == SETT_BTYPE_ALL ||
                    settings.esp_building_type == SETT_BTYPE_SENTRY)
                    building_esp(ent, "Sentry", col_build_friend,
                                 col_build_enemy);

                break;
            }

            case CClass_CObjectDispenser: {
                if (settings.esp_building_type == SETT_BTYPE_ALL ||
                    settings.esp_building_type == SETT_BTYPE_DISPENSER)
                    building_esp(ent, "Dispenser", col_build_friend,
                                 col_build_enemy);

                break;
            }

            case CClass_CObjectTeleporter: {
                if (settings.esp_building_type == SETT_BTYPE_ALL ||
                    settings.esp_building_type == SETT_BTYPE_TELEPORTER) {
                    const char* name = (ent->m_iObjectMode == 0)
                                         ? "Teleporter entry"
                                         : "Teleporter exit";
                    building_esp(ent, name, col_build_friend, col_build_enemy);
                }

                break;
            }

            case CClass_CTFGrenadePipebombProjectile: {
                if (settings.esp_sticky == SETT_OFF || !IsStickyBomb(ent))
                    break;

                /* Get player that owns the stiky. We could render other info */
                CBaseHandle thrower_handle = GetThrowerHandle(ent);
                if (!CBaseHandle_IsValid(thrower_handle))
                    continue;

                int thrower_idx = CBaseHandle_GetEntryIndex(thrower_handle);
                Entity* thrower = g.ents[thrower_idx];
                if (!thrower)
                    continue;

                const int ent_teamnum  = METHOD(thrower, GetTeamNumber);
                const bool is_teammate = (our_teamnum == ent_teamnum);

                /* We don't want to render this team */
                if (settings.esp_sticky != SETT_ALL &&
                    ((settings.esp_sticky == SETT_FRIEND && !is_teammate) ||
                     (settings.esp_sticky == SETT_ENEMY && is_teammate)))
                    continue;

                rgba_t col_sticky = settings.esp_sticky_use_team_color
                                      ? NK2COL(get_team_color(ent_teamnum))
                                    : is_teammate ? col_sticky_friend
                                                  : col_sticky_enemy;

                generic_ent_name(ent, "Sticky", col_sticky);
                break;
            }

            case CClass_CTFAmmoPack: {
                /* Dropped ammo, not the normal ammo boxes */
                if (settings.esp_ammobox)
                    generic_ent_name(ent, "Ammo", col_ammobox);
                break;
            }

            case CClass_CCurrencyPack: {
                /* MvM money. Not adding another option/color for this shit */
                if (settings.esp_ammobox)
                    generic_ent_name(ent, "Currency", col_ammobox);
                break;
            }

            case CClass_CBaseAnimating: {
                /* Ammo boxes and healing items are all CBaseAnimating, so we
                 * need to check the model */

                bool drawed_health = false;

                if (settings.esp_healthpack) {
                    /* Check that this model is in the healthpack range */
                    for (int j = MDLIDX_MEDKIT_SMALL;
                         j <= MDLIDX_MUSHROOM_LARGE; j++) {
                        /* Compare against our model cache.
                         * See cache_get_model_idx() */
                        if (ent->model_idx == g.mdl_idx[j]) {
                            generic_ent_name(ent, "Health", col_healthpack);
                            drawed_health = true;
                            break;
                        }
                    }
                }

                if (!drawed_health && settings.esp_ammobox) {
                    /* Check that this model is in the ammopack range */
                    for (int j = MDLIDX_AMMOPACK_SMALL;
                         j <= MDLIDX_AMMOPACK_SMALL_BDAY; j++) {
                        if (ent->model_idx == g.mdl_idx[j]) {
                            generic_ent_name(ent, "Ammo", col_ammobox);
                            break;
                        }
                    }
                }

                /* end: case CClass_CBaseAnimating */
                break;
            }

            case CClass_CTFTankBoss: {
                if (settings.esp_player != SETT_ALL &&
                    settings.esp_player != SETT_ENEMY)
                    continue;

                if (!get_bbox(ent, &x, &y, &w, &h))
                    continue;

                /* Only working settings for MvM tanks are box and name esp */
                if (settings.esp_player_box)
                    OUTLINED_BOX(x, y, w, h, col_player_enemy);

                if (settings.esp_player_name || settings.esp_player_class) {
                    int infopos_x = x + w / 2;
                    int infopos_y = y + h + 2;

                    draw_text(infopos_x, infopos_y, true, g_fonts.main.id,
                              col_player_enemy, "Tank");
                }

                /* end: case CClass_CTFTankBoss */
                break;
            }

            default:
                break;
        } /* end: Entity class switch */
    }     /* end: Entity for loop */
} /* end: esp() */
