#ifndef SETTINGS_H_
#define SETTINGS_H_

#define OFF 0

enum settings_rage {
    LEGIT = 1,
    RAGE  = 2,
};

enum settings_friendly {
    FRIENDLY = 1,
    ENEMY    = 2,
    ALL      = 3,
};

typedef struct {
    /* Movement */
    int bhop;
    int autostrafe;

    /* ESP */
    int enable_esp;
    int box_esp;
    int health_esp;
    int name_esp;
    int class_esp;
    int weapon_esp;

    /* Misc */
    int watermark;
    int speclist;
    int autostab;
} Settings;

extern Settings settings;

#endif /* SETTINGS_H_ */
