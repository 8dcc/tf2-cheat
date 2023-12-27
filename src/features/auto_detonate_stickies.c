
#include "features.h"
#include "../include/sdk.h"
#include "../include/math.h"
#include "../include/globals.h"

#define CLEAR_SPAWN_CACHE_AND_RETURN() \
    {                                  \
        spawn_cache_pos = 0;           \
        return;                        \
    }

#define MAX_STICKIES 8

typedef struct {
    int idx;
    float spawn_time;
} SpawnData;

static SpawnData spawn_cache[MAX_STICKIES];
static int spawn_cache_pos = 0;

/*----------------------------------------------------------------------------*/

static inline float get_arm_time(int item_def_index) {
    return (item_def_index == Demoman_s_TheQuickiebombLauncher) ? 0.6f : 0.8f;
}

static void spawn_cache_update(int* indexes, int sz) {
    /* We use this method to clear all the unused stickies in the cache, and
     * only keep the real ones from indexes[] */
    SpawnData new_cache[MAX_STICKIES];
    int new_cache_pos = 0;

    /* Clear new cache */
    for (int i = 0; i < MAX_STICKIES; i++) {
        new_cache[i].idx        = 0;
        new_cache[i].spawn_time = 0.f;
    }

    /* Iterate input indexes */
    for (int i = 0; i < sz && new_cache_pos < MAX_STICKIES; i++) {
        /* Is it already stored in the spawn cache? */
        float spawn_time = 0.f;
        for (int j = 0; j < spawn_cache_pos; j++) {
            if (indexes[i] == spawn_cache[j].idx) {
                spawn_time = spawn_cache[j].spawn_time;
                break;
            }
        }

        new_cache[new_cache_pos].idx = indexes[i];

        /* If this sticky was already in the spawn cache, use that spawn time.
         * Otherwise, store it spawned now */
        new_cache[new_cache_pos].spawn_time =
          (spawn_time > 0.f) ? spawn_time : c_globalvars->curtime;

        new_cache_pos++;
    }

    memcpy(spawn_cache, new_cache, sizeof(new_cache));
    spawn_cache_pos = new_cache_pos;
}

static bool spawn_cache_can_detonate(int ent_idx, int item_def_index) {
    float spawn_time = 0.f;

    /* Check if we saved the spawn time of this sticky */
    for (int i = 0; i < spawn_cache_pos; i++) {
        if (spawn_cache[i].idx == ent_idx) {
            spawn_time = spawn_cache[i].spawn_time;
            break;
        }
    }

    /* Did not find spawn time for sticky, store it just spawned */
    if (spawn_time == 0.f)
        return false;

    /* Return true if N seconds have passed since spawn time */
    const float arm_time = get_arm_time(item_def_index);
    return c_globalvars->curtime >= spawn_time + arm_time;
}

/*----------------------------------------------------------------------------*/

void auto_detonate_stickies(usercmd_t* cmd) {
    if (!settings.auto_detonate || !g.localplayer || !g.localweapon ||
        !g.IsAlive)
        CLEAR_SPAWN_CACHE_AND_RETURN();

    /* User is manually detonating */
    if (cmd->buttons & IN_ATTACK2)
        return;

    /* Get weapon in second slot. We have to iterate because the index in
     * `m_hMyWeapons' is not always one (if we use boots, for example). */
    Weapon* secondary = NULL;

    for (int i = 0; i <= 2; i++) {
        const CBaseHandle handle = g.localplayer->m_hMyWeapons[i];
        if (!CBaseHandle_IsValid(handle))
            continue;

        const int idx      = CBaseHandle_GetEntryIndex(handle);
        Weapon* cur_weapon = (Weapon*)g.ents[idx];
        if (!cur_weapon)
            continue;

        /* Check if we have launcher in second slot */
        const int cur_weapon_id = METHOD(cur_weapon, GetWeaponId);
        if (cur_weapon_id == TF_WEAPON_PIPEBOMBLAUNCHER) {
            secondary = cur_weapon;
            break;
        }
    }

    /* We didn't find a valid TF_WEAPON_PIPEBOMBLAUNCHER in our weapons */
    if (!secondary)
        CLEAR_SPAWN_CACHE_AND_RETURN();

    /* Used in previous 2 functions to check the arm time of the stickies */
    const int wpn_item_def_idx = secondary->m_iItemDefinitionIndex;

    int sticky_indexes[MAX_STICKIES] = { 0 };
    int sticky_indexes_pos           = 0;

    /* Iterate entities, searching for stickies */
    for (int i = g.MaxClients + 1; i < g.MaxEntities; i++) {
        Entity* sticky = g.ents[i];
        if (!sticky)
            continue;

        Networkable* sticky_net   = GetNetworkable(sticky);
        ClientClass* sticky_class = METHOD(sticky_net, GetClientClass);
        if (!sticky_class)
            continue;

        /* CTFGrenadePipebombProjectile means both stickies and pipebombs */
        if (sticky_class->class_id != CClass_CTFGrenadePipebombProjectile)
            continue;

        /* Is it a sticky or a pipebomb? */
        if (!IsStickyBomb(sticky))
            continue;

        /* Are we the owner of the stickybomb? */
        CBaseHandle thrower_handle = GetThrowerHandle(sticky);
        if (!CBaseHandle_IsValid(thrower_handle))
            continue;

        const int thrower_idx = CBaseHandle_GetEntryIndex(thrower_handle);
        if (thrower_idx != g.localidx)
            continue;

        /* We don't have space left in the array */
        if (sticky_indexes_pos >= MAX_STICKIES)
            break;

        /* One of our stickies, store */
        sticky_indexes[sticky_indexes_pos++] = i;
    }

    /* No stickies, clear spawn cache and return */
    if (sticky_indexes_pos == 0)
        CLEAR_SPAWN_CACHE_AND_RETURN();

    /* Update spawn cache with our sticky list. If some stickies are not in the
     * spawn cache, it will add them. */
    spawn_cache_update(sticky_indexes, sticky_indexes_pos);

    /* Iterate our stickies again */
    for (int i = 0; i < sticky_indexes_pos; i++) {
        const int ent_idx = sticky_indexes[i];

        /* Check if the sticky is armed */
        if (!spawn_cache_can_detonate(ent_idx, wpn_item_def_idx))
            continue;

        /* We already validated the entity when adding it to the array */
        Entity* sticky = g.ents[ent_idx];

        /* Valid sticky. Calculate position once. */
        const vec3_t sticky_pos = *METHOD(sticky, WorldSpaceCenter);

        /* Current entity is one of our stickies, check if it's close enough to
         * an enemy. */
        for (int j = 1; j <= g.MaxClients; j++) {
            Entity* player               = g.ents[j];
            plist_player_t* plist_player = &g.playerlist[j];
            if (!player || !plist_player || !plist_player->is_good ||
                IsTeammate(player) || !METHOD(player, IsAlive))
                continue;

            Networkable* net = GetNetworkable(player);
            if (METHOD(net, IsDormant))
                continue;

            /* If we are the current player, just check if we want to detonate
             * ourselves. Otherwise, also make sure the player is a vulnerable
             * enemy. */
            if (j == g.localidx) {
                if (!settings.auto_detonate_self)
                    continue;
            } else {
                if (IsTeammate(player))
                    continue;

                if (!settings.aim_target_invul && IsInvulnerable(player))
                    continue;

                if (plist_player->is_ignored)
                    continue;

                if (!settings.aim_target_friends &&
                    (plist_player->is_steam_friend ||
                     plist_player->preset == FRIEND))
                    continue;

                if (!settings.aim_target_invisible && IsInvisible(player))
                    continue;
            }

            /* TODO: If scotish resistance, look to the sticky entity with
             * g.pSilent */

            /* Is it close enough? */
            const vec3_t player_pos = GetCenter(player);
            const float distance    = vec_len(vec_sub(sticky_pos, player_pos));

            if (distance > settings.auto_detonate_dist)
                continue;

            /* Is there anything between the sticky and the target? */
            if (is_enemy_visible(sticky_pos, player_pos, player)) {
                cmd->buttons |= IN_ATTACK2;
                return;
            }
        }
    }
}
