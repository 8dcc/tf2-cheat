#ifndef SDK_STUDIOHDR_H_
#define SDK_STUDIOHDR_H_

/* NOTE: This file is included from sdk.h and depends on its declarations */

enum ETFHitbox {
    /* Head */
    HITBOX_HEAD,

    /* Torso */
    HITBOX_PELVIS,
    HITBOX_SPINE0,
    HITBOX_SPINE1,
    HITBOX_SPINE2,
    HITBOX_SPINE3,

    /* Arms */
    HITBOX_LEFT_UPPER_ARM,
    HITBOX_LEFT_LOWER_ARM,
    HITBOX_LEFT_HAND,
    HITBOX_RIGHT_UPPER_ARM,
    HITBOX_RIGHT_LOWER_ARM,
    HITBOX_RIGHT_HAND,

    /* Legs */
    HITBOX_LEFT_HIP,
    HITBOX_LEFT_KNEE,
    HITBOX_LEFT_FOOT,
    HITBOX_RIGHT_HIP,
    HITBOX_RIGHT_KNEE,
    HITBOX_RIGHT_FOOT,

    HITBOX_MAX,
};

typedef struct {
    int bone;
    int group;
    vec3_t bbmin;
    vec3_t bbmax;
    int szhitboxnameindex;
    int unused[8];
} studiobbox_t;

typedef struct {
    int sznameindex;
    /* pszName */
    int numhitboxes;
    int hitboxindex;
    /* pHitbox */
} studiohitboxset_t;

static inline studiobbox_t* studiohitboxset_pHitbox(studiohitboxset_t* thisptr,
                                                    int i) {
    return (studiobbox_t*)(((void*)thisptr) + thisptr->hitboxindex) + i;
};

typedef struct {
    int sznameindex;
    int parent;
    int bonecontroller[6];
    vec3_t pos;
    vec4_t quat;
    vec3_t rot; /* RadianEuler */
    vec3_t posscale;
    vec3_t rotscale;
    matrix3x4_t poseToBone;
    vec4_t qAlignment;
    int flags;
    int proctype;
    int procindex;
    int physicsbone;
    int surfacepropidx;
    int contents;
    int unused[8];
} studiobone_t;

struct studiohdr_t {
    int id;
    int version;
    int checksum;
    /* pszName() */
    char name[64];
    int length;
    vec3_t eyeposition;
    vec3_t illumposition;
    vec3_t hull_min;
    vec3_t hull_max;
    vec3_t view_bbmin;
    vec3_t view_bbmax;
    int flags;
    int numbones;
    int boneindex;
    /* pBone(int i) */
    int numbonecontrollers;
    int bonecontrollerindex;
    int numhitboxsets;
    int hitboxsetindex;
    /* ... */
};

static inline studiobone_t* studiohdr_pBone(studiohdr_t* thisptr, const int i) {
    if (i < 0 || i >= thisptr->numbones)
        return NULL;

    return (studiobone_t*)(((void*)thisptr) + thisptr->boneindex) + i;
}

static inline studiohitboxset_t* studiohdr_pHitboxSet(studiohdr_t* thisptr,
                                                      int i) {
    if (i < 0 || i >= thisptr->numhitboxsets)
        return NULL;

    return (studiohitboxset_t*)(((void*)thisptr) + thisptr->hitboxsetindex) + i;
}

/* Wrapper for studiohdr_pHitboxSet and studiohitboxset_pHitbox */
static inline studiobbox_t* studiohdr_pHitbox(studiohdr_t* thisptr, int set,
                                              int idx) {
    studiohitboxset_t* hitboxset = studiohdr_pHitboxSet(thisptr, set);
    if (!hitboxset)
        return NULL;

    return (studiobbox_t*)studiohitboxset_pHitbox(hitboxset, idx);
}

#endif /* SDK_STUDIOHDR_H_ */
