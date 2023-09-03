#ifndef SDK_ENTITY_H_
#define SDK_ENTITY_H_

/* NOTE: This file is included from sdk.h and depends on its declarations */

#define MAX_PLAYER_NAME_LENGTH 32
#define SIGNED_GUID_LEN        32
#define MAX_CUSTOM_FILES       4

typedef struct player_info_s {
    char name[MAX_PLAYER_NAME_LENGTH];
    int userID;
    char guid[SIGNED_GUID_LEN + 1];
    uint32_t friendsID;
    char friendsName[MAX_PLAYER_NAME_LENGTH];
    bool fakeplayer;
    bool ishltv;
    uint32_t customFiles[MAX_CUSTOM_FILES];
    unsigned char filesDownloaded;
} player_info_t;

/*----------------------------------------------------------------------------*/

typedef struct Collideable Collideable;
typedef struct Networkable Networkable;
typedef struct Renderable Renderable;
typedef struct Entity Entity;
typedef struct Weapon Weapon;

typedef struct {
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
    PAD(0x8);
    const char* network_name;
    PAD(0x8);
    int class_id; /* enum ETFClientClass */
} ClientClass;

typedef struct {
    PAD(4 * 2);
    ClientClass* (*GetClientClass)(Networkable*); /* 2 */
    PAD(4 * 5);
    bool (*IsDormant)(Networkable*); /* 8 */
    int (*GetIndex)(Networkable*);   /* 9 */
} VMT_Networkable;

struct Networkable {
    VMT_Networkable* vmt;
};

struct model_t {
    void* handle;
    char* name;
    int load_flags;
    int server_count;
    int type;
    int flags;
    vec3_t vec_mins;
    vec3_t vec_maxs;
    float radius;
};

#define MAXSTUDIOBONES 128 /* Size of "bones" array */
typedef struct {
    PAD(4 * 9);
    const model_t* (*GetModel)(Renderable*); /* 9 */
    PAD(4 * 6);
    bool (*SetupBones)(Renderable*, matrix3x4_t* bones, int maxBones,
                       int boneMask, float currentTime); /* 16 */
    PAD(4 * 17);
    matrix3x4_t* (*RenderableToWorldTransform)(Renderable*); /* 34 */
} VMT_Renderable;

struct Renderable {
    VMT_Renderable* vmt;
};

typedef struct {
    PAD(4 * 4);
    Collideable* (*GetCollideable)(Entity*); /* 4 */
    PAD(4 * 6);
    vec3_t* (*GetAbsOrigin)(Entity*); /* 11 */
    vec3_t* (*GetAbsAngles)(Entity*); /* 12 */
    PAD(4 * 66);
    int (*GetIndex)(Entity*); /* 79 */
    PAD(4 * 26);
    vec3_t* (*WorldSpaceCenter)(Entity*); /* 106 */
    PAD(4 * 10);
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
    bool (*IsWeapon)(Entity*); /* 190 */
    PAD(4 * 3);
    vec3_t (*EyePosition)(Entity*); /* 194 */
    vec3_t* (*EyeAngles)(Entity*);  /* 195 */
    PAD(4 * 95);
    Weapon* (*GetWeapon)(Entity*); /* 291 */
    PAD(4 * 10);
    vec3_t (*GetShootPos)(Entity*); /* 302 */
    PAD(4 * 6);
    int (*GetObserverMode)(Entity*);       /* 309 */
    Entity* (*GetObserverTarget)(Entity*); /* 310 */
} VMT_Entity;

/* NOTE: Most of these offsets are from the game's NetVars, and have been dumped
 * using 8dcc/source-netvar-dumper. Until I figure out a good netvar system in
 * C, it might be a good idea to re-dump them on each game update. */
struct Entity {
    VMT_Entity* vmt;
    PAD(0x7C);
    int model_idx; /* 0x80 */
    PAD(0x8C);
    vec3_t velocity; /* 0x110 */
    PAD(0x250);
    int flags; /* 0x36C */
    PAD(0x8E4);
    float flNextAttack; /* 0xC54 */
    PAD(0x214);
    vec3_t vecPunchAngle; /* 0xE6C */
    PAD(0x2EC);
    usercmd_t* m_pCurrentCommand; /* 0x1164, see CPrediction::StartCommand() */
    PAD(0xCC);
    int nTickBase; /* 0x1234 */
    PAD(0x3F8);
    int player_class; /* 0x1630 (ETFClass) */
    PAD(0x7D0);
    int nForceTauntCam; /* 0x1E04 */
};

/*----------------------------------------------------------------------------*/

static inline int CBaseHandle_IsValid(CBaseHandle h) {
    return h != INVALID_EHANDLE_INDEX;
}

static inline int CBaseHandle_GetEntryIndex(CBaseHandle h) {
    return h & ENT_ENTRY_MASK;
}

#define IsTeammate(ent) \
    (METHOD(g.localplayer, GetTeamNumber) == METHOD(ent, GetTeamNumber))

static inline Renderable* GetRenderable(Entity* ent) {
    return (Renderable*)((void*)ent + 0x4);
}

static inline Networkable* GetNetworkable(Entity* ent) {
    return (Networkable*)((void*)ent + 0x8);
}

static inline const char* GetClassName(Entity* ent) {
    switch (ent->player_class) {
        case CLASS_SCOUT:
            return "Scout";
        case CLASS_DEMOMAN:
            return "Demoman";
        case CLASS_ENGINEER:
            return "Engineer";
        case CLASS_HEAVY:
            return "Heavy";
        case CLASS_MEDIC:
            return "Medic";
        case CLASS_PYRO:
            return "Pyro";
        case CLASS_SNIPER:
            return "Sniper";
        case CLASS_SOLDIER:
            return "Soldier";
        case CLASS_SPY:
            return "Spy";
        default:
            return "Unknown class";
    }
}

#endif /* SDK_ENTITY_H_ */
