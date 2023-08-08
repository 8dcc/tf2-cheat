
#include "features.h"
#include "../include/sdk.h"
#include "../include/globals.h"

#define OUTLINED_BOX(x0, y0, x1, y1, c)                               \
    METHOD_ARGS(i_surface, SetColor, 0, 0, 0, 0);                     \
    METHOD_ARGS(i_surface, DrawRect, x0 - 1, y0 - 1, x1 + 1, y1 + 1); \
    METHOD_ARGS(i_surface, DrawRect, x0 + 1, y0 + 1, x1 - 1, y1 - 1); \
    METHOD_ARGS(i_surface, SetColor, c.r, c.g, c.b, c.a);             \
    METHOD_ARGS(i_surface, DrawRect, x0, y0, x1, y1);

static bool get_bbox(Entity* ent, int* x, int* y, int* w, int* h) {
    Collideable* collideable = METHOD(ent, GetCollideable);
    if (!collideable)
        return false;

    vec3_t obb_mins = *METHOD(collideable, ObbMinsPreScaled);
    vec3_t obb_maxs = *METHOD(collideable, ObbMaxsPreScaled);
    /* TODO: entity->coordinate_frame() */

    vec3_t points[] = { { obb_mins.x, obb_mins.y, obb_mins.z },
                        { obb_mins.x, obb_maxs.y, obb_mins.z },
                        { obb_maxs.x, obb_maxs.y, obb_mins.z },
                        { obb_maxs.x, obb_mins.y, obb_mins.z },
                        { obb_maxs.x, obb_maxs.y, obb_maxs.z },
                        { obb_mins.x, obb_maxs.y, obb_maxs.z },
                        { obb_mins.x, obb_mins.y, obb_maxs.z },
                        { obb_maxs.x, obb_mins.y, obb_maxs.z } };

    for (int i = 0; i < 8; i++) {
        vec3_t t = points[i];

        /* TODO */
        /* math::transform_vector(points[i], trans, t); */

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
    if (!localplayer)
        return;

    /* For bounding box */
    int x, y, w, h;

    /* Iterate all entities */
    for (int i = 1; i <= 32; i++) {
        Entity* ent      = METHOD_ARGS(i_entitylist, GetClientEntity, i);
        Networkable* net = GetNetworkable(ent);

        if (!ent || !net || !METHOD(ent, IsAlive) || METHOD(net, IsDormant) ||
            IsLocalplayer(ent))
            continue;

        if (!get_bbox(ent, &x, &y, &w, &h))
            continue;

        const bool teammate = IsTeammate(ent);
        if ((teammate && settings.box_esp & FRIENDLY) ||
            (!teammate && settings.box_esp & ENEMY)) {
            const rgba_t col = teammate ? (rgba_t){ 10, 240, 10, 255 }
                                        : (rgba_t){ 240, 10, 10, 255 };

            OUTLINED_BOX(x, y, x + w, y + h, col);
        }

        /* TODO: Name esp, etc. */
    }
}
