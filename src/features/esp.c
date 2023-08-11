
#include "features.h"
#include "../include/sdk.h"
#include "../include/globals.h"

#define INFOPOS_LINE_H 11

#define OUTLINED_BOX(x, y, w, h, c)                                           \
    {                                                                         \
        METHOD_ARGS(i_surface, SetColor, 0, 0, 0, 255);                       \
        METHOD_ARGS(i_surface, DrawRect, x - 1, y - 1, x + w + 1, y + h + 1); \
        METHOD_ARGS(i_surface, DrawRect, x + 1, y + 1, x + w - 1, y + h - 1); \
        METHOD_ARGS(i_surface, SetColor, c.r, c.g, c.b, c.a);                 \
        METHOD_ARGS(i_surface, DrawRect, x, y, x + w, y + h);                 \
    }

static bool get_bbox(Entity* ent, int* x, int* y, int* w, int* h) {
    Collideable* collideable = METHOD(ent, GetCollideable);
    if (!collideable)
        return false;

    vec3_t obb_mins = *METHOD(collideable, ObbMins);
    vec3_t obb_maxs = *METHOD(collideable, ObbMaxs);

    Renderable* renderable = GetRenderable(ent);
    if (!renderable)
        return false;

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

void esp(void) {
    if (settings.enable_esp == OFF)
        return;

    if (!g.localplayer || !g.IsConnected)
        return;

    /* For bounding box */
    int x, y, w, h;

    /* Iterate all entities */
    for (int i = 1; i <= g.MaxClients; i++) {
        if (i == g.localidx)
            continue;

        Entity* ent      = g.ents[i];
        Networkable* net = GetNetworkable(ent);
        if (!ent)
            continue;

        ClientClass* ent_class = METHOD(net, GetClientClass);
        if (!ent_class)
            continue;

        switch (ent_class->class_id) {
            case CClass_CTFPlayer: {
                const bool teammate = IsTeammate(ent);

                /* Should we render this player's team? */
                if (settings.enable_esp != ALL &&
                    ((teammate && settings.enable_esp == ENEMY) ||
                     (!teammate && settings.enable_esp == FRIENDLY)))
                    continue;

                if (!get_bbox(ent, &x, &y, &w, &h))
                    continue;

                const rgba_t col = teammate ? (rgba_t){ 13, 120, 243, 255 }
                                            : (rgba_t){ 244, 27, 24, 255 };

                /*------------------------------------------------------------*/
                /* Player box ESP */

                if (settings.box_esp)
                    OUTLINED_BOX(x, y, w, h, col);

                /*------------------------------------------------------------*/
                /* Player health ESP */

                if (settings.health_esp) {
                    const int hp     = METHOD(ent, GetHealth);
                    const int max_hp = METHOD(ent, GetMaxHealth);

                    const int hpx = x - 5;
                    const int hpy = y;
                    const int hpw = 2;
                    const int hph = h;

                    /* Background (red) */
                    METHOD_ARGS(i_surface, SetColor, 0, 0, 0, 255);
                    METHOD_ARGS(i_surface, DrawRect, hpx - 1, hpy - 1,
                                hpx + hpw + 1, hpy + h + 1);
                    METHOD_ARGS(i_surface, DrawRect, hpx + 1, hpy + 1,
                                hpx + hpw - 1, hpy + h - 1);
                    METHOD_ARGS(i_surface, SetColor, 170, 29, 29, 255);
                    METHOD_ARGS(i_surface, DrawFilledRect, hpx, hpy, hpx + hpw,
                                hpy + hph);

                    /* Health bar (green) */
                    const int hpbar_h = (hph * MIN(hp, max_hp) / max_hp);
                    const int hpbar_y = hpy + (hph - hpbar_h);
                    METHOD_ARGS(i_surface, SetColor, 67, 160, 71, 255);
                    METHOD_ARGS(i_surface, DrawFilledRect, hpx, hpbar_y,
                                hpx + hpw, hpbar_y + hpbar_h);
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

                    infopos_y += INFOPOS_LINE_H;
                }

                /*------------------------------------------------------------*/
                /* Player class ESP */

                if (settings.class_esp) {
                    draw_text(infopos_x, infopos_y, true, g_fonts.main.id,
                              col_scale(col, 1.30f), GetClassName(ent));

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
                                  col_scale(col, 1.50f), wname);

                        infopos_y += INFOPOS_LINE_H;
                    }
                }

                break;
            }
            default:
                break;
        }
    }
}
