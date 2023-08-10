
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
        fprintf(stderr, "load: error loading globals, aborting\n");
        self_unload();
    }

    fonts_init();

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

    if (!hooks_restore()) {
        fprintf(stderr, "unload: error restoring hooks, aborting\n");
        self_unload();
    }

    printf("Enoch unloaded.\n\n");
}

void self_unload(void) {
    void* self = dlopen("libenoch.so", RTLD_LAZY | RTLD_NOLOAD);
    dlclose(self); /* Close the call we just made to dlopen() */
    dlclose(self); /* Close the call our injector made */
}
