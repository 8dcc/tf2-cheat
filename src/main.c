#include "include/netvars.h"
#include <stdbool.h>
#include <stdio.h>
#define __USE_GNU
#include <dlfcn.h>
#include "include/main.h"
#include "include/globals.h"
#include "include/hooks.h"

static bool globals_loaded          = false;
static bool hooks_loaded            = false;
static const char* library_filename = NULL;

__attribute__((constructor)) /* Entry point when injected */
void load(void) {
    printf("Enoch injected!\n");

    /* Get correct library filename for self_unload() */
    Dl_info dl_info;
    if (dladdr((void*)load, &dl_info) != 0)
        library_filename = dl_info.dli_fname;
    else
        ERR("Couldn't get library_filename, self_unload() will fail!");

    globals_loaded = globals_init();
    if (!globals_loaded) {
        ERR("Error loading globals, aborting");
        self_unload();
    }

    fonts_init();

    hooks_loaded = hooks_init();
    if (!hooks_loaded) {
        ERR("Error loading hooks, aborting");
        self_unload();
    }
}

__attribute__((destructor)) /* Entry point when unloaded */
void unload() {
    if (globals_loaded && !resore_vtables()) {
        ERR("Error restoring vtables, aborting");
        return;
    }

    if (g_netvars != NULL)
        netvars_free(g_netvars);

    if (hooks_loaded && !hooks_restore()) {
        ERR("Error restoring hooks, aborting");
        return;
    }

    globals_loaded = false;
    hooks_loaded   = false;

    printf("Enoch unloaded.\n\n");
}

void self_unload(void) {
    if (!library_filename)
        return;

    void* self = dlopen(library_filename, RTLD_LAZY | RTLD_NOLOAD);
    if (!self)
        return;

    dlclose(self); /* Close the call we just made to dlopen() */
    dlclose(self); /* Close the call our injector made */
}
