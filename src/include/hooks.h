#ifndef HOOKS_H_
#define HOOKS_H_

#include <stdbool.h>
#include "sdk.h"

/* NOTE: For commented version, see:
 *   https://github.com/8dcc/hl-cheat/blob/main/src/include/hooks.h */

#define DECL_HOOK_EXTERN(type, name, ...)  \
    typedef type (*name##_t)(__VA_ARGS__); \
    extern name##_t ho_##name;             \
    type h_##name(__VA_ARGS__);

#define DECL_HOOK(name) name##_t ho_##name = NULL;

#define HOOK(interface, name)          \
    ho_##name       = interface->name; \
    interface->name = h_##name;

#define ORIGINAL(name, ...) ho_##name(__VA_ARGS__);

/*----------------------------------------------------------------------------*/

bool hooks_init(void);

DECL_HOOK_EXTERN(bool, CreateMove, ClientMode*, float, usercmd_t*);

#endif /* HOOKS_H_ */
