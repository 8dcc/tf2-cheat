
#include "features.h"
#include "../include/sdk.h"
#include "../include/globals.h"

void autobackstab(usercmd_t* cmd) {
    if (!settings.autostab || !g.localplayer || !g.IsAlive)
        return;

    Weapon* weapon = METHOD(g.localplayer, GetWeapon);
    if (!weapon)
        return;

    Networkable* net       = GetNetworkable((Entity*)weapon);
    ClientClass* ent_class = METHOD(net, GetClientClass);
    if (!ent_class || ent_class->class_id != CClass_CTFKnife)
        return;

    /* TODO: Fix and use IsBehindAndFacingTarget
     * https://www.unknowncheats.me/forum/2897712-post287.html */
    if (weapon->ready_to_backstab)
        cmd->buttons |= IN_ATTACK;
}

/*----------------------------------------------------------------------------*/

#define SPEC_LINE_H 14

void spectator_list(void) {
    if (!settings.speclist || !g.localplayer || !g.IsInGame)
        return;

    /* If we are dead, display spectator list for the guy we are spectating */
    Entity* local = g.IsAlive ? g.localplayer
                              : METHOD(g.localplayer, GetObserverTarget);
    if (!local)
        return;

    int spec_x = 0, spec_y = 0;
    METHOD_ARGS(i_engine, GetScreenSize, &spec_x, &spec_y);

    spec_y = spec_y / 2 - 50;
    spec_x = 5;

    bool first_spectator = true;

    int maxclients = MIN(64, g.MaxClients);
    for (int i = 1; i <= maxclients; i++) {
        if (i == g.localidx)
            continue;

        Entity* ent      = METHOD_ARGS(i_entitylist, GetClientEntity, i);
        Networkable* net = GetNetworkable(ent);
        if (!ent || ent == g.localplayer || ent == local ||
            METHOD(net, IsDormant) || METHOD(ent, IsAlive))
            continue;

        /* Not spectating us */
        const int obs_mode = METHOD(ent, GetObserverMode);
        if (obs_mode == OBS_MODE_NONE ||
            METHOD(ent, GetObserverTarget) != local)
            continue;

        if (first_spectator) {
            draw_text(spec_x, spec_y, false, g_fonts.main.id,
                      (rgba_t){ 65, 186, 70, 255 }, "Spectators:");

            spec_y += SPEC_LINE_H + 1;
            first_spectator = false;
        }

        /* Add name to list */
        player_info_t pinfo;
        if (!METHOD_ARGS(i_engine, GetPlayerInfo, i, &pinfo))
            continue;

        static char converted[sizeof("[Freeze] ") + MAX_PLAYER_NAME_LENGTH];

        /* j will track position inside converted[], k inside pinfo.name[] */
        int j = 0, k = 0;

        switch (obs_mode) {
            default:
            case OBS_MODE_IN_EYE:
                strcpy(converted, "[1st] ");
                j += sizeof("[1st] ") - 1;
                break;
            case OBS_MODE_CHASE:
                strcpy(converted, "[3st] ");
                j += sizeof("[3st] ") - 1;
                break;
            case OBS_MODE_ROAMING:
                strcpy(converted, "[Free] ");
                j += sizeof("[Free] ") - 1;
                break;
            case OBS_MODE_FREEZECAM:
                strcpy(converted, "[Freeze] ");
                j += sizeof("[Freeze] ") - 1;
                break;
            case OBS_MODE_DEATHCAM:
                strcpy(converted, "[Death] ");
                j += sizeof("[Death] ") - 1;
                break;
            case OBS_MODE_FIXED:
                strcpy(converted, "[Fixed] ");
                j += sizeof("[Fixed] ") - 1;
                break;
            case OBS_MODE_POI: /* Point of interest (game objective, etc.) */
                strcpy(converted, "[Poi] ");
                j += sizeof("[Poi] ") - 1;
                break;
        }

        do {
            uint32_t c     = pinfo.name[k];
            converted[j++] = c <= 0x7F ? c : '?';
        } while (pinfo.name[k++] != '\0');

        draw_text(spec_x, spec_y, false, g_fonts.main.id,
                  (rgba_t){ 200, 200, 200, 255 }, converted);

        spec_y += SPEC_LINE_H;
    }
}
