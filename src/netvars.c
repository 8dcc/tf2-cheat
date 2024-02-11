#include "include/netvars.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "include/sdk.h"
#include "include/globals.h"

#define NODES_INIT \
    hashmap_new(sizeof(node_t), 0, 0, 0, node_hash, node_compare, NULL, NULL)

int node_compare(const void* a, const void* b, void* udata) {
    const node_t* na = a;
    const node_t* nb = b;
    return strcmp(na->name, nb->name);
}

uint64_t node_hash(const void* item, uint64_t seed0, uint64_t seed1) {
    const node_t* node = item;
    return hashmap_sip(node->name, strlen(node->name), seed0, seed1);
}

bool node_init(node_t* node, RecvTable* table, uint32_t offset) {
    if (!node || !table || !table->tableName || table->tableName[0] == '_')
        return false;

    node->offset = offset;
    node->name   = table->tableName;

    if (table->propsCount > 0) {
        node->nodes = NODES_INIT;
        if (!node->nodes) {
            return false;
        }
    }

    for (int i = 0; i < table->propsCount; i++) {
        const RecvProp* prop = &table->props[i];

        if (!prop || !prop->varName || isdigit(prop->varName[0]) ||
            prop->varName[0] == '_' || !strcmp(prop->varName, "baseclass"))
            continue;

        const uint32_t prop_offset = offset + prop->offset;
        node_t node_new;
        memset(&node_new, 0, sizeof(node_t));
        node_new.name   = prop->varName;
        node_new.offset = prop_offset;

        // printf("prop %d name %s offset %d\n", i, prop->varName, prop_offset);

        if (prop->recvType == DATATABLE && prop->dataTable) {
            node_new.nodes = NODES_INIT;
            if (node_new.nodes) {
                /*printf("Trying to initialize subnode %s\n",
                       prop->dataTable->tableName);*/
                node_init(&node_new, prop->dataTable, prop_offset);
            }
        }

        hashmap_set(node->nodes, &node_new);
    }

    return true;
}

struct hashmap* netvars_init() {
    struct hashmap* netvars = NODES_INIT;
    if (!netvars)
        return NULL;

    for (ClientClass* client_class  = METHOD(i_baseclient, GetAllClasses);
         client_class; client_class = client_class->next) {
        if (client_class && client_class->recv_table) {
            node_t node;
            memset(&node, 0, sizeof(node_t));
            /*printf("trying to initialize node %s\n",
                   client_class->recv_table->tableName);*/
            if (node_init(&node, client_class->recv_table, 0) == true) {
                /*printf("node %s initialized succesfully\n",
                       client_class->recv_table->tableName);*/
                hashmap_set(netvars, &node);
            }
        }
    }

    return netvars;
}

void netvars_dump(struct hashmap* netvars) {
    if (!netvars)
        return;

    size_t iter = 0;
    void* item;
    while (hashmap_iter(netvars, &iter, &item)) {
        const node_t* node = item;
        if (node->name)
            printf("%zu %s (%d)\n", iter, node->name, node->offset);
        else {
            printf("%zu unnamed node (%d)\n", iter, node->offset);
        }
        if (node->nodes)
            netvars_dump(node->nodes);
    }
}

void netvars_free(struct hashmap* netvars) {
    if (!netvars)
        return;

    size_t iter = 0;
    void* item;
    while (hashmap_iter(netvars, &iter, &item)) {
        const node_t* node = item;
        /*if (node->name)
            printf("%zu Freeing node %s\n", iter, node->name);*/
        if (node->nodes) {
            /*printf("%zu node has %zu subnodes\n", iter,
                   hashmap_count(node->nodes));*/
            netvars_free(node->nodes);
        }
    }

    hashmap_free(netvars);
}

bool netvars_get(struct hashmap* netvars, const char* name, node_t** netvar) {
    if (!netvars || !name)
        return false;

    size_t iter = 0;
    void* item;
    while (hashmap_iter(netvars, &iter, &item)) {
        node_t* node = item;
        if (node->name && strcmp(node->name, name) == 0) {
            *netvar = node;
            return true;
        }
    }
    return false;
}

uint32_t va_netvars_get_offset_recursive(struct hashmap* netvars, int amount,
                                         va_list argp) {
    if (amount > 0) {
        char* arg = va_arg(argp, char*);
        amount -= 1;

        node_t* netvar = NULL;
        if (netvars_get(netvars, arg, &netvar) && netvar) {
            if (amount > 0) {
                if (netvar->nodes)
                    return va_netvars_get_offset_recursive(netvar->nodes,
                                                           amount, argp);
                else {
                    ERR("Can't go recursive on netvar %s, recursion amount: %d "
                        "but nodes "
                        "are null",
                        arg, amount);
                    exit(1);
                }
            } else {
                return netvar->offset;
            }
        } else {
            ERR("Failed to get netvar %s", arg);
            exit(1);
        }
    }

    return 0;
}

uint32_t netvars_get_offset_recursive(struct hashmap* netvars, int amount,
                                      ...) {
    va_list argp;
    va_start(argp, amount);

    const uint32_t result =
      va_netvars_get_offset_recursive(netvars, amount, argp);

    va_end(argp);

    return result;
}
