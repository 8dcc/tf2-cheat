#ifndef WEAPON_H_
#define WEAPON_H_

typedef struct Weapon Weapon;

typedef struct {
    PAD(4 * 400);
    const char* (*GetName)(Weapon*);
} VMT_Weapon;

struct Weapon {
    VMT_Weapon* vmt;
    PAD(0xC3C);
    bool ready_to_backstab; /* 0xC40 */
    /* uint16_t item_definition_index; /\* 0x928 *\/ */
};

#endif    // WEAPON_H_
