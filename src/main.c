
#include <stdbool.h>
#include <stdio.h>
#include <dlfcn.h>

static bool loaded = false;

__attribute__((constructor)) /* Entry point when injected */
void load(void) {
    printf("tf2-cheat injected!\n");

    loaded = true;
}

__attribute__((destructor)) /* Entry point when unloaded */
void unload() {
    if (!loaded)
        return;

    /* TODO: Unhook stuff */

    printf("tf2-cheat unloaded.\n\n");
}

void self_unload(void) {
    void* self = dlopen("libtf2cheat.so", RTLD_LAZY | RTLD_NOLOAD);

    /* Close the call we just made to dlopen() */
    dlclose(self);

    /* Close the call our injector made */
    dlclose(self);
}
