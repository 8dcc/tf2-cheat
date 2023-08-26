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
    PAD(4 * 74);
    int (*GetSwingRange)(Weapon*); /* 525 */
} VMT_Weapon;

struct Weapon {
    VMT_Weapon* vmt;
    PAD(0xA34);
    CBaseHandle hOwner; /* 0xA38 */
    PAD(0x10);
    float flNextPrimaryAttack; /* 0xA4C */
    PAD(0x1DC);
    float smackTime; /* 0xC2C, see CTFWeaponBaseMelee::ItemPostFrame() */
    PAD(0x10);
    bool bReadyToBackstab; /* 0xC40 */
};

#endif /* SDK_WEAPON_H_ */
