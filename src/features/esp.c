
#include "features.h"
#include "../include/sdk.h"
#include "../include/globals.h"

#define OUTLINED_BOX(x, y, w, h, c)                                       \
    METHOD_ARGS(i_surface, SetColor, 0, 0, 0, 255);                       \
    METHOD_ARGS(i_surface, DrawRect, x - 1, y - 1, x + w + 1, y + h + 1); \
    METHOD_ARGS(i_surface, DrawRect, x + 1, y + 1, x + w - 1, y + h - 1); \
    METHOD_ARGS(i_surface, SetColor, c.r, c.g, c.b, c.a);                 \
    METHOD_ARGS(i_surface, DrawRect, x, y, x + w, y + h);

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

void player_esp(void) {
    if (settings.box_esp == OFF)
        return;

    if (!localplayer || !METHOD(i_engine, IsInGame) ||
        !METHOD(i_engine, IsConnected))
        return;

    /* For bounding box */
    int x, y, w, h;

    /* Iterate all entities */
    for (int i = 1; i <= 32; i++) {
        if (i == METHOD(i_engine, GetLocalPlayer))
            continue;

        Entity* ent      = METHOD_ARGS(i_entitylist, GetClientEntity, i);
        Networkable* net = GetNetworkable(ent);
        if (!ent || METHOD(net, IsDormant))
            continue;

        ClientClass* ent_class = METHOD(net, GetClientClass);
        if (!ent_class)
            continue;

        switch (ent_class->class_id) {
            case CClass_CTFPlayer: {
                if (!get_bbox(ent, &x, &y, &w, &h))
                    continue;

                if (!METHOD(ent, IsAlive))
                    continue;

                const bool teammate = IsTeammate(ent);
                if ((teammate && settings.box_esp & FRIENDLY) ||
                    (!teammate && settings.box_esp & ENEMY)) {
                    const rgba_t col = teammate ? (rgba_t){ 10, 240, 10, 255 }
                                                : (rgba_t){ 240, 10, 10, 255 };

                    OUTLINED_BOX(x, y, w, h, col);
                }

                /* TODO: Name esp, etc. */
                /* Weapon* weapon = METHOD(ent, GetWeapon); */

                /* player_info_t pinfo; */
                /* METHOD_ARGS(i_engine, GetPlayerInfo, i, &pinfo); */

                break;
            }
            default:
                break;
        }
    }
}
