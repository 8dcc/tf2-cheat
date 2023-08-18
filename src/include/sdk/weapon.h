#ifndef SDK_WEAPON_H_
#define SDK_WEAPON_H_

/* NOTE: This file is included from sdk.h and depends on its declarations */

typedef struct Weapon Weapon;

typedef struct {
    PAD(4 * 398);
    int (*GetSlot)(Weapon*); /* 398 */
    PAD(4 * 1);
    const char* (*GetName)(Weapon*); /* 400 */
    PAD(4 * 48);
    int (*GetWeaponId)(Weapon*);   /* 449 */
    int (*GetDamageType)(Weapon*); /* 450 */
} VMT_Weapon;

struct Weapon {
    VMT_Weapon* vmt;
    PAD(0xA48);
    float flNextPrimaryAttack; /* 0xA4C */
    PAD(0x1F0);
    bool bReadyToBackstab; /* 0xC40 */
};

#endif /* SDK_WEAPON_H_ */
