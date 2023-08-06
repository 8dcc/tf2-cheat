#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdbool.h>
#include <string.h> /* CLONE_VTABLE: memcpy */
#include <stdlib.h> /* CLONE_VTABLE: malloc */
#include "util.h"
#include "sdk.h"

/*
 * NOTE: For commented version, see:
 *   https://github.com/8dcc/hl-cheat/blob/main/src/include/hooks.h
 *   https://github.com/8dcc/bms-cheat/blob/main/src/include/hooks.h
 *
 * prefix | meaning
 * -------+-------------------------------
 * h_*    | handler ptr (global scope)
 * i_*    | interface ptr (global scope)
 * oVTi_* | original vtable pointer (will be replaced with our own vtable)
 * nVTi_* | new vtable pointer allocated by us
 */
#define DECL_INTF(type, name)      \
    type* i_##name         = NULL; \
    VT_##type* oVTi_##name = NULL; \
    VT_##type* nVTi_##name = NULL;

#define DECL_INTF_EXTERN(type, name) \
    extern type* i_##name;           \
    extern VT_##type* oVTi_##name;   \
    extern VT_##type* nVTi_##name;

#define CLONE_VTABLE(class, name)                                           \
    oVT##name = name->vt;                                                   \
    nVT##name = malloc(vmt_size(name->vt));                                 \
    if (!nVT##name) {                                                       \
        fprintf(stderr, "CLONE_VTABLE: Could not allocate vtable for %s\n", \
                #name);                                                     \
        return false;                                                       \
    }                                                                       \
    memcpy(nVT##name, name->vt, vmt_size(name->vt));                        \
    name->vt = nVT##name;

#define RESTORE_VTABLE(class, name) \
    name->vt = oVT##name;           \
    free(nVT##name);

/*----------------------------------------------------------------------------*/
/* Global variables */

extern void* h_client;

DECL_INTF_EXTERN(BaseClient, baseclient);
DECL_INTF_EXTERN(ClientMode, clientmode);

/*----------------------------------------------------------------------------*/
/* Functions from globals.c */

bool globals_init(void);
bool resore_vtables(void);

#endif /* GLOBALS_H_ */
