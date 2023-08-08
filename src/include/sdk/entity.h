#ifndef ENTITY_H_
#define ENTITY_H_

enum entity_flags {
    FL_ONGROUND              = (1 << 0),
    FL_DUCKING               = (1 << 1),
    FL_WATERJUMP             = (1 << 2),
    FL_ONTRAIN               = (1 << 3),
    FL_INRAIN                = (1 << 4),
    FL_FROZEN                = (1 << 5),
    FL_ATCONTROLS            = (1 << 6),
    FL_CLIENT                = (1 << 7),
    FL_FAKECLIENT            = (1 << 8),
    FL_INWATER               = (1 << 9),
    FL_FLY                   = (1 << 10),
    FL_SWIM                  = (1 << 11),
    FL_CONVEYOR              = (1 << 12),
    FL_NPC                   = (1 << 13),
    FL_GODMODE               = (1 << 14),
    FL_NOTARGET              = (1 << 15),
    FL_AIMTARGET             = (1 << 16),
    FL_PARTIALGROUND         = (1 << 17),
    FL_STATICPROP            = (1 << 18),
    FL_GRAPHED               = (1 << 19),
    FL_GRENADE               = (1 << 20),
    FL_STEPMOVEMENT          = (1 << 21),
    FL_DONTTOUCH             = (1 << 22),
    FL_BASEVELOCITY          = (1 << 23),
    FL_WORLDBRUSH            = (1 << 24),
    FL_OBJECT                = (1 << 25),
    FL_KILLME                = (1 << 26),
    FL_ONFIRE                = (1 << 27),
    FL_DISSOLVING            = (1 << 28),
    FL_TRANSRAGDOLL          = (1 << 29),
    FL_UNBLOCKABLE_BY_PLAYER = (1 << 30)
};

/*----------------------------------------------------------------------------*/

typedef struct Collideable Collideable;
typedef struct Networkable Networkable;
typedef struct Entity Entity;
typedef struct Weapon Weapon;
extern Entity* localplayer;

typedef struct {
    /* REVIEW: Prescalled or normal */
    PAD(4 * 1);
    vec3_t* (*ObbMinsPreScaled)(Collideable*); /* 1 */
    vec3_t* (*ObbMaxsPreScaled)(Collideable*); /* 2 */
    vec3_t* (*ObbMins)(Collideable*);          /* 3 */
    vec3_t* (*ObbMaxs)(Collideable*);          /* 4 */
} VMT_Collideable;

struct Collideable {
    VMT_Collideable* vmt;
};

typedef struct {
    PAD(4 * 8);
    bool (*IsDormant)(Networkable*); /* 8 */
    int (*GetIndex)(Networkable*);   /* 9 */
} VMT_Networkable;

struct Networkable {
    VMT_Networkable* vmt;
};

typedef struct {
    PAD(4 * 4);
    Collideable* (*GetCollideable)(Entity*); /* 4 */
    PAD(4 * 6);
    vec3_t* (*GetAbsOrigin)(Entity*); /* 11 */
    vec3_t* (*GetAbsAngles)(Entity*); /* 12 */
    PAD(4 * 66);
    int (*GetIndex)(Entity*); /* 79 */
    PAD(4 * 37);
    int (*GetTeamNumber)(Entity*); /* 117 */
    PAD(4 * 34);
    int (*GetHealth)(Entity*);    /* 152 */
    int (*GetMaxHealth)(Entity*); /* 153 */
    PAD(4 * 29);
    bool (*IsAlive)(Entity*);  /* 183 */
    bool (*IsPlayer)(Entity*); /* 184 */
    PAD(4 * 2);
    bool (*IsNPC)(Entity*); /* 187 */
    PAD(4 * 2);
    bool (*IsWeapon)(Entity*);     /* 190 */
    Weapon* (*GetWeapon)(Entity*); /* 191 */
    PAD(4 * 2);
    vec3_t (*EyePosition)(Entity*); /* 194 */
    vec3_t (*EyeAngles)(Entity*);   /* 195 */
} VMT_Entity;

struct Entity {
    VMT_Entity* vmt;
    PAD(0x10C);
    vec3_t velocity; /* 0x110 */
    PAD(0x250);
    int flags; /* 0x36C */
};

static inline void* GetRendereable(Entity* ent) {
    /* TODO: Rendereable struct */
    return (Networkable*)((void*)ent + 0x4);
}

static inline Networkable* GetNetworkable(Entity* ent) {
    return (Networkable*)((void*)ent + 0x8);
}

static inline bool IsLocalplayer(Entity* ent) {
    return METHOD(localplayer, GetIndex) == METHOD(i_engine, GetLocalPlayer);
}

static inline bool IsTeammate(Entity* ent) {
    return METHOD(localplayer, GetTeamNumber) == METHOD(ent, GetTeamNumber);
}

#endif /* ENTITY_H_ */
