#ifndef HOOKS_H_
#define HOOKS_H_

#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "sdk.h"
#include "globals.h"

/* NOTE: For commented version, see:
 *   https://github.com/8dcc/hl-cheat/blob/main/src/include/hooks.h
 *
 * Table of prefixes:
 *   prefix  | meaning
 *   --------+----------------------------
 *   *_t     | typedef (function type)
 *   h_*     | hook function (ours)
 *   ho_*    | hook original (ptr to orig)
 */

#define DECL_HOOK_EXTERN(type, name, ...)  \
    typedef type (*name##_t)(__VA_ARGS__); \
    extern name##_t ho_##name;             \
    type h_##name(__VA_ARGS__);

#define DECL_HOOK(name) name##_t ho_##name = NULL;

#define VMT_HOOK(interface, name)                \
    ho_##name            = interface->vmt->name; \
    interface->vmt->name = h_##name;

#define ORIGINAL(name, ...) ho_##name(__VA_ARGS__);

#define HOOK_SDL(name)       \
    ho_##name  = *name##Ptr; \
    *name##Ptr = h_##name;

/*----------------------------------------------------------------------------*/

bool hooks_init(void);
bool hooks_restore(void);

DECL_HOOK_EXTERN(void, LevelShutdown, BaseClient*);
DECL_HOOK_EXTERN(void, LevelInitPostEntity, BaseClient*);
DECL_HOOK_EXTERN(void, FrameStageNotify, BaseClient*, ClientFrameStage_t);
DECL_HOOK_EXTERN(bool, CreateMove, ClientMode*, float, usercmd_t*);
DECL_HOOK_EXTERN(void, OverrideView, ClientMode*, ViewSetup*);
DECL_HOOK_EXTERN(void, Paint, EngineVGui*, uint32_t);
DECL_HOOK_EXTERN(void, PaintTraverse, IPanel*, VPanel, bool, bool);
DECL_HOOK_EXTERN(void, DrawModelExecute, ModelRender*, DrawModelState_t*,
                 ModelRenderInfo_t*, matrix3x4_t*);
DECL_HOOK_EXTERN(void, RunCommand, IPrediction*, Entity*, usercmd_t*,
                 MoveHelper*);
DECL_HOOK_EXTERN(usercmd_t*, GetUserCmd, CInput*, int);
DECL_HOOK_EXTERN(void, OnScreenSizeChanged, MatSurface*, int, int);

extern SwapWindow_t ho_SwapWindow;
extern PollEvent_t ho_PollEvent;
void h_SwapWindow(SDL_Window* window);
int h_PollEvent(SDL_Event* event);

#endif /* HOOKS_H_ */
