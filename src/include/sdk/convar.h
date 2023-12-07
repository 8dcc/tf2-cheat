
#ifndef SDK_CONVAR_H_
#define SDK_CONVAR_H_

/* TODO */
typedef void* ConCommand;

typedef struct {
    PAD(4 * 14);
    void (*InternalSetValue)(ConVar*, const char* value);            /* 14 */
    void (*InternalSetFloatValue)(ConVar*, float value, bool force); /* 15 */
    void (*InternalSetIntValue)(ConVar*, int value);                 /* 16 */
} ConVar_VMT;

struct ConVar {
    ConVar_VMT* vmt;
    PAD(0x18);
    ConVar* m_pParent; /* 28 */
    const char* m_pszDefaultValue;
    char* m_pszString;
    int m_StringLength;
    float m_fValue;
    int m_nValue; /* 48, see ConVar::InternalSetValue() */
    bool m_bHasMin;
    float m_fMinVal;
    bool m_bHasMax;
    float m_fMaxVal;
    bool m_bHasCompMin;
    float m_fCompMinVal;
    bool m_bHasCompMax;
    float m_fCompMaxVal;
    bool m_bCompetitiveRestrictions;
    void* m_fnChangeCallback; /* FnChangeCallback_t */
};

static inline void ConVar_SetFloat(ConVar* thisptr, float val) {
    ConVar* parent = thisptr->m_pParent;
    METHOD_ARGS(parent, InternalSetFloatValue, val, false);
}

static inline void ConVar_SetInt(ConVar* thisptr, int val) {
    ConVar* parent = thisptr->m_pParent;
    METHOD_ARGS(parent, InternalSetIntValue, val);
}

static inline void ConVar_SetString(ConVar* thisptr, const char* val) {
    ConVar* parent = thisptr->m_pParent;
    METHOD_ARGS(parent, InternalSetValue, val);
}

static inline float ConVar_GetFloat(ConVar* thisptr) {
    return thisptr->m_pParent->m_fValue;
}

static inline int ConVar_GetInt(ConVar* thisptr) {
    return thisptr->m_pParent->m_nValue;
}

static inline const char* ConVar_GetString(ConVar* thisptr) {
    return (thisptr->m_pParent->m_pszString != NULL)
             ? thisptr->m_pParent->m_pszString
             : "";
}

#endif /* SDK_CONVAR_H_ */
