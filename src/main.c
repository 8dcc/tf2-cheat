
#include <stdbool.h>
#include <stdio.h>
#include <dlfcn.h>
#include "include/main.h"
#include "include/globals.h"
#include "include/hooks.h"

static bool loaded = false;

__attribute__((constructor)) /* Entry point when injected */
void load(void) {
    printf("Enoch injected!\n");

    if (!globals_init()) {
        ERR("Error loading globals, aborting");
        self_unload();
    }

    fonts_init();

    if (!hooks_init()) {
        ERR("Error loading hooks, aborting");
        self_unload();
    }

    loaded = true;
}

__attribute__((destructor)) /* Entry point when unloaded */
void unload() {
    if (!loaded)
        return;

    if (!resore_vtables()) {
        ERR("Error restoring vtables, aborting");
        self_unload();
    }

    if (!hooks_restore()) {
        ERR("Error restoring hooks, aborting");
        self_unload();
    }

    printf("Enoch unloaded.\n\n");
}

void self_unload(void) {
    /* FIXME: I think this function always crashes because it is looking for our
     * lib inside the game's foler. Find a way of adding absolute path here? */
    void* self = dlopen("libenoch.so", RTLD_LAZY | RTLD_NOLOAD);
    if (!self)
        return;

    dlclose(self); /* Close the call we just made to dlopen() */
    dlclose(self); /* Close the call our injector made */
}
