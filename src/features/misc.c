
#include "features.h"
#include "../include/sdk.h"
#include "../include/globals.h"

void autobackstab(usercmd_t* cmd) {
    if (!settings.autostab || !g.IsInGame || !localplayer ||
        !METHOD(localplayer, IsAlive))
        return;

    Weapon* weapon = METHOD(localplayer, GetWeapon);
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
