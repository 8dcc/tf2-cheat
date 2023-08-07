#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <stdbool.h>

enum settings_autostrafe {
    OFF   = 0,
    LEGIT = 1,
    RAGE  = 2,
};

typedef struct {
    int bhop;
    int autostrafe;
} Settings;

extern Settings settings;

#endif /* SETTINGS_H_ */
