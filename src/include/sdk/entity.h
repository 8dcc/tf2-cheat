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
    PAD(4 * 12);
    void (*ThirdPersonSwitch)(Entity*, bool bThirdperson); /* 208 */
    PAD(4 * 82);
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
    PAD(0x7C);
    uint8_t m_nWaterLevel; /* 0x198, gcc adds 3 bytes of padding */
    PAD(0x1B8);
    vec3_t m_vecOrigin; /* 0x354 */
    PAD(0xC);
    int flags; /* 0x36C */
    PAD(0x8E4);
    float flNextAttack; /* 0xC54 */
    PAD(0x84);
    CBaseHandle m_hMyWeapons[MAX_WEAPONS]; /* 0xCDC */
    PAD(0xD0);                             /* Starts at 0xD9C */
    vec3_t vecPunchAngle;                  /* 0xE6C */
    PAD(0xD0);
    int m_iObjectMode; /* 0xF48 */
    PAD(0x1C4);
    vec3_t v_angle; /* 0x1110, From CPlayerState. See `player->pl.v_angle' in
                       CPrediction::GetLocalViewAngles() */
    PAD(0x48);
    usercmd_t* m_pCurrentCommand; /* 0x1164, see CPrediction::StartCommand() */
    PAD(0xCC);
    int nTickBase; /* 0x1234 */
    PAD(0x3F8);
    int player_class; /* 0x1630 (ETFClass) */
    PAD(0x36C);
    int m_nPlayerCond;    /* 0x19A0 */
    int m_nPlayerCondEx;  /* 0x19A4 */
    int m_nPlayerCondEx2; /* 0x19A8 */
    int m_nPlayerCondEx3; /* 0x19AC */
    int m_nPlayerCondEx4; /* 0x19B0 */
    PAD(0x18);
    int condition_bits; /* 0x19CC */
    PAD(0x418);
    bool m_bAllowMoveDuringTaunt; /* 0x1DE8 */
    PAD(0x18);
    int nForceTauntCam; /* 0x1E04 */
};

/*----------------------------------------------------------------------------*/

/* Use macros if we use g.localplayer, we don't want to include globals.h */
#define IsTeammate(ENT) \
    (METHOD(g.localplayer, GetTeamNumber) == METHOD(ENT, GetTeamNumber))

#define IsSteamFriend(ENT) (IsPlayerOnSteamFriendsList(g.localplayer, ENT))

static inline int CBaseHandle_IsValid(CBaseHandle h) {
    return h != INVALID_EHANDLE_INDEX;
}

static inline int CBaseHandle_GetEntryIndex(CBaseHandle h) {
    return h & ENT_ENTRY_MASK;
}

static inline Renderable* GetRenderable(Entity* ent) {
    return (Renderable*)((uint32_t)ent + 0x4);
}

static inline Networkable* GetNetworkable(Entity* ent) {
    return (Networkable*)((uint32_t)ent + 0x8);
}

static inline int GetMoveType(Entity* ent) {
    /* Got the offset from the top of CBaseEntity::VPhysicsUpdate() */
    const int offset = 0x194;
    return *(int*)((uint32_t)ent + offset);
}

/* NOTE: Caller should check if `ent' is a CTFGrenadePipebombProjectile */
static inline bool IsStickyBomb(Entity* ent) {
    /* CTFGrenadePipebombProjectile->m_iType */
    const int offset  = 0x8DC;
    int pipebomb_type = *(int*)((uint32_t)ent + offset);

    /* Normal sticky or jumper sticky */
    return pipebomb_type == TF_GL_MODE_REMOTE_DETONATE ||
           pipebomb_type == TF_GL_MODE_REMOTE_DETONATE_PRACTICE;
}

static inline CBaseHandle GetThrowerHandle(Entity* ent) {
    /* CBaseGrenade->m_hThrower */
    const int offset = 0x8B4;
    return *(CBaseHandle*)((uint32_t)ent + offset);
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

static inline vec3_t GetCenter(Entity* ent) {
    Collideable* collideable = METHOD(ent, GetCollideable);
    if (!collideable)
        return VEC_ZERO;

    vec3_t mins = *METHOD(collideable, ObbMins);
    vec3_t maxs = *METHOD(collideable, ObbMaxs);

    vec3_t ret = *METHOD(ent, GetAbsOrigin);
    ret.z += (mins.z + maxs.z) / 2.0f;

    return ret;
}

static inline bool InCond(Entity* ent, enum ETFCond cond) {
    switch (cond / 32) {
        case 0: {
            const int bit = (1 << cond);
            return ((ent->m_nPlayerCond & bit) == bit ||
                    (ent->condition_bits & bit) == bit);
        }
        case 1: {
            const int bit = 1 << (cond - 32);
            return ((ent->m_nPlayerCondEx & bit) == bit);
        }
        case 2: {
            const int bit = 1 << (cond - 64);
            return ((ent->m_nPlayerCondEx2 & bit) == bit);
        }
        case 3: {
            const int bit = 1 << (cond - 96);
            return ((ent->m_nPlayerCondEx3 & bit) == bit);
        }
        case 4: {
            const int bit = 1 << (cond - 128);
            return ((ent->m_nPlayerCondEx4 & bit) == bit);
        }
        default:
            return false;
    }
}

static inline bool IsInvulnerable(Entity* ent) {
    return InCond(ent, TF_COND_INVULNERABLE) ||
           InCond(ent, TF_COND_INVULNERABLE_CARD_EFFECT) ||
           InCond(ent, TF_COND_INVULNERABLE_HIDE_UNLESS_DAMAGE) ||
           InCond(ent, TF_COND_INVULNERABLE_USER_BUFF) ||
           InCond(ent, TF_COND_PHASE);
}

static inline bool IsInvisible(Entity* ent) {
    if (InCond(ent, TF_COND_BURNING) || InCond(ent, TF_COND_BURNING_PYRO) ||
        InCond(ent, TF_COND_MAD_MILK) || InCond(ent, TF_COND_URINE))
        return false;

    /* CTFPlayer->m_flInvisChangeCompleteTime - 0x8 */
    const int invis_offset = 0x1A14;
    float m_flInvisibility = *(float*)((uint32_t)ent + invis_offset);
    return m_flInvisibility >= 1.f;
}

static inline bool IsCritBoosted(Entity* ent) {
    return InCond(ent, TF_COND_CRITBOOSTED) ||
           InCond(ent, TF_COND_CRITBOOSTED_BONUS_TIME) ||
           InCond(ent, TF_COND_CRITBOOSTED_CARD_EFFECT) ||
           InCond(ent, TF_COND_CRITBOOSTED_CTF_CAPTURE) ||
           InCond(ent, TF_COND_CRITBOOSTED_FIRST_BLOOD) ||
           InCond(ent, TF_COND_CRITBOOSTED_ON_KILL) ||
           InCond(ent, TF_COND_CRITBOOSTED_PUMPKIN) ||
           InCond(ent, TF_COND_CRITBOOSTED_RAGE_BUFF) ||
           InCond(ent, TF_COND_CRITBOOSTED_RUNE_TEMP) ||
           InCond(ent, TF_COND_CRITBOOSTED_USER_BUFF);
}

static inline bool IsMiniCritBoosted(Entity* ent) {
    return InCond(ent, TF_COND_MINICRITBOOSTED_ON_KILL) ||
           InCond(ent, TF_COND_NOHEALINGDAMAGEBUFF) ||
           InCond(ent, TF_COND_ENERGY_BUFF);
}

static inline bool IsMarked(Entity* ent) {
    /* We treat urine and mark as the same in ESP */
    return InCond(ent, TF_COND_URINE) || InCond(ent, TF_COND_MARKEDFORDEATH) ||
           InCond(ent, TF_COND_MARKEDFORDEATH_SILENT);
}

#endif /* SDK_ENTITY_H_ */
