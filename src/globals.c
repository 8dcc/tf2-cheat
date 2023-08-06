
#include <stdio.h>
#include <dlfcn.h>
#include "include/globals.h"

#define CLIENT_SO "./tf/bin/client.so"

#define GET_HANDLER(VAR, STR)                                   \
    VAR = dlopen(STR, RTLD_LAZY | RTLD_NOLOAD);                 \
    if (!VAR) {                                                 \
        fprintf(stderr, "globals_init: can't open " #VAR "\n"); \
        return false;                                           \
    }

#define GET_INTERFACE(TYPE, VAR, HANDLER, STR)                   \
    VAR = (TYPE)get_interface(HANDLER, STR);                     \
    if (!VAR || !VAR->vt) {                                      \
        fprintf(stderr, "globals_init: cant't load " #VAR "\n"); \
        return false;                                            \
    }

/*----------------------------------------------------------------------------*/

void* h_client = NULL;

DECL_INTF(BaseClient, baseclient);
DECL_INTF(ClientMode, clientmode);

/*----------------------------------------------------------------------------*/

/* See: https://github.com/8dcc/bms-cheat/wiki */
static inline ClientMode* get_clientmode(void) {
    const int byte_offset = 1;

    void* func_ptr      = i_baseclient->vt->HudProcessInput;
    void* g_pClientMode = *(void**)(func_ptr + byte_offset); /* 60 1F 06 02 */
    ClientMode* ret     = *(ClientMode**)g_pClientMode;

    return ret;
}

bool globals_init(void) {
    /* Handlers */
    GET_HANDLER(h_client, CLIENT_SO);

    /* Interfaces */
    GET_INTERFACE(BaseClient*, i_baseclient, h_client, "VClient017");

    /* Other interfaces */
    i_clientmode = get_clientmode();
    if (!i_clientmode || !i_clientmode->vt) {
        fprintf(stderr, "globals_init: couldn't load i_clientmodebms\n");
        return false;
    }

    CLONE_VTABLE(ClientMode, i_clientmode);

    return true;
}

bool resore_vtables(void) {
    RESTORE_VTABLE(ClientMode, i_clientmode);

    return true;
}
