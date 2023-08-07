#ifndef ENTITY_H_
#define ENTITY_H_

typedef struct Entity Entity;

typedef struct {
    PAD(4 * 11);
    vec3_t* (*GetAbsOrigin)(Entity*); /* 11 */
    vec3_t* (*GetAbsAngles)(Entity*); /* 12 */
} VMT_Entity;

struct Entity {
    VMT_Entity* vmt;
    PAD(0x368);
    int m_fFlags; /* 0x36C */
};

#endif /* ENTITY_H_ */
