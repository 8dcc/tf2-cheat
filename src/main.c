
#include <stdbool.h>
#include <stdio.h>
#include <dlfcn.h>
#include "include/main.h"
#include "include/globals.h"
#include "include/hooks.h"

static bool loaded = false;

__attribute__((constructor)) /* Entry point when injected */
void load(void) {
    printf("tf2-cheat injected!\n");

    if (!globals_init()) {
        fprintf(stderr, "load: error loading globals, aborting\n");
        self_unload();
    }

    if (!hooks_init()) {
        fprintf(stderr, "load: error loading hooks, aborting\n");
        self_unload();
    }

    loaded = true;
}

__attribute__((destructor)) /* Entry point when unloaded */
void unload() {
    if (!loaded)
        return;

    if (!resore_vtables()) {
        fprintf(stderr, "unload: error restoring vtables, aborting\n");
        self_unload();
    }

    printf("tf2-cheat unloaded.\n\n");
}

void self_unload(void) {
    void* self = dlopen("libtf2cheat.so", RTLD_LAZY | RTLD_NOLOAD);

    /* Close the call we just made to dlopen() */
    dlclose(self);

    /* Close the call our injector made */
    dlclose(self);
}
