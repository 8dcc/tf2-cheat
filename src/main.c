#include <stdbool.h>
#include <stdio.h>
#define __USE_GNU
#include <dlfcn.h>
#include "include/main.h"
#include "include/globals.h"
#include "include/hooks.h"

static bool loaded                  = false;
static const char* library_filename = "";

__attribute__((constructor)) /* Entry point when injected */
void load(void) {
    printf("Enoch injected!\n");

    /* Get library filename for self_unload() */
    Dl_info dl_info;
    if (dladdr((void*)load, &dl_info) != 0) {
        library_filename = dl_info.dli_fname;
        // printf("Enoch library path: %s", library_filename);
    } else {
        ERR("Warning: Couldn't get library_filename, self_unload() will fail!");
    }

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
    if (!library_filename || strcmp(library_filename, "") == 0)
        return;

    void* self = dlopen(library_filename, RTLD_LAZY | RTLD_NOLOAD);
    if (!self)
        return;

    dlclose(self); /* Close the call we just made to dlopen() */
    dlclose(self); /* Close the call our injector made */
}
