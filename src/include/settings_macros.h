
/*------------------------------------------------------*
 * Take a ride on the wild side of the C preprocessor   *
 *------------------------------------------------------*/

#define SAVE_ABORT(...)               \
    {                                 \
        fprintf(stderr, __VA_ARGS__); \
        cJSON_Delete(json_cfg);       \
        return;                       \
    }

#define JSON_SETTINGS_WRITE_INT(PARENT, ITEM)                   \
    if (!cJSON_AddNumberToObject(PARENT, #ITEM, settings.ITEM)) \
        SAVE_ABORT("save_config: error saving \"settings.%s\"\n", #ITEM);

#define JSON_SETTINGS_WRITE_FLOAT(PARENT, ITEM) \
    JSON_SETTINGS_WRITE_INT(PARENT, ITEM)

/* Create json_ITEM object with rbga float values, add that object to PARENT */
#define JSON_SETTINGS_WRITE_COL(PARENT, ITEM)                        \
    cJSON* json_##ITEM = cJSON_CreateObject();                       \
    if (!cJSON_AddNumberToObject(json_##ITEM, "r", settings.ITEM.r)) \
        SAVE_ABORT("save_config: error saving red value for "        \
                   "\"settings.%s\"\n",                              \
                   #ITEM);                                           \
    if (!cJSON_AddNumberToObject(json_##ITEM, "g", settings.ITEM.g)) \
        SAVE_ABORT("save_config: error saving red value for "        \
                   "\"settings.%s\"\n",                              \
                   #ITEM);                                           \
    if (!cJSON_AddNumberToObject(json_##ITEM, "b", settings.ITEM.b)) \
        SAVE_ABORT("save_config: error saving red value for "        \
                   "\"settings.%s\"\n",                              \
                   #ITEM);                                           \
    if (!cJSON_AddNumberToObject(json_##ITEM, "a", settings.ITEM.a)) \
        SAVE_ABORT("save_config: error saving red value for "        \
                   "\"settings.%s\"\n",                              \
                   #ITEM);                                           \
    if (!cJSON_AddItemToObject(PARENT, #ITEM, json_##ITEM))          \
        SAVE_ABORT("save_config: error adding rgba object for "      \
                   "\"settings.%s\"\n",                              \
                   #ITEM);

/*----------------------------------------------------------------------------*/

#define LOAD_PRINT_ERROR()                                         \
    {                                                              \
        fprintf(stderr, "load_config: %s\n", cJSON_GetErrorPtr()); \
        free(json_cfg_str);                                        \
        cJSON_Delete(json_cfg);                                    \
        return;                                                    \
    }

#define LOAD_ABORT(...)               \
    {                                 \
        fprintf(stderr, __VA_ARGS__); \
        free(json_cfg_str);           \
        cJSON_Delete(json_cfg);       \
        return;                       \
    }

#define JSON_SETTINGS_READ_INT(PARENT, ITEM)                                  \
    do {                                                                      \
        cJSON* json_##ITEM = cJSON_GetObjectItemCaseSensitive(PARENT, #ITEM); \
        if (!cJSON_IsNumber(json_##ITEM)) {                                   \
            fprintf(stderr, "WARNING: setting \"%s\" (number) not found\n",   \
                    #ITEM);                                                   \
            break;                                                            \
        }                                                                     \
        settings.ITEM = json_##ITEM->valueint;                                \
    } while (0);

#define JSON_SETTINGS_READ_FLOAT(PARENT, ITEM)                                \
    do {                                                                      \
        cJSON* json_##ITEM = cJSON_GetObjectItemCaseSensitive(PARENT, #ITEM); \
        if (!cJSON_IsNumber(json_##ITEM)) {                                   \
            fprintf(stderr, "WARNING: setting \"%s\" (float) not found\n",    \
                    #ITEM);                                                   \
            break;                                                            \
        }                                                                     \
        settings.ITEM = json_##ITEM->valuedouble;                             \
    } while (0);

#define JSON_SETTINGS_READ_COL(PARENT, ITEM)                                  \
    do {                                                                      \
        cJSON* json_##ITEM = cJSON_GetObjectItemCaseSensitive(PARENT, #ITEM); \
        if (!cJSON_IsObject(json_##ITEM)) {                                   \
            fprintf(stderr, "WARNING: setting \"%s\" (rgba obj) not found\n", \
                    #ITEM);                                                   \
            break;                                                            \
        }                                                                     \
        cJSON* json_##ITEM##_r =                                              \
          cJSON_GetObjectItemCaseSensitive(json_##ITEM, "r");                 \
        if (!cJSON_IsNumber(json_##ITEM##_r)) {                               \
            fprintf(stderr, "WARNING: setting \"%s\" (rgba.r) not found\n",   \
                    #ITEM);                                                   \
            break;                                                            \
        }                                                                     \
        cJSON* json_##ITEM##_g =                                              \
          cJSON_GetObjectItemCaseSensitive(json_##ITEM, "g");                 \
        if (!cJSON_IsNumber(json_##ITEM##_g)) {                               \
            fprintf(stderr, "WARNING: setting \"%s\" (rgba.g) not found\n",   \
                    #ITEM);                                                   \
            break;                                                            \
        }                                                                     \
        cJSON* json_##ITEM##_b =                                              \
          cJSON_GetObjectItemCaseSensitive(json_##ITEM, "b");                 \
        if (!cJSON_IsNumber(json_##ITEM##_b)) {                               \
            fprintf(stderr, "WARNING: setting \"%s\" (rgba.b) not found\n",   \
                    #ITEM);                                                   \
            break;                                                            \
        }                                                                     \
        cJSON* json_##ITEM##_a =                                              \
          cJSON_GetObjectItemCaseSensitive(json_##ITEM, "a");                 \
        if (!cJSON_IsNumber(json_##ITEM##_a)) {                               \
            fprintf(stderr, "WARNING: setting \"%s\" (rgba.a) not found\n",   \
                    #ITEM);                                                   \
            break;                                                            \
        }                                                                     \
        settings.ITEM.r = json_##ITEM##_r->valuedouble;                       \
        settings.ITEM.g = json_##ITEM##_g->valuedouble;                       \
        settings.ITEM.b = json_##ITEM##_b->valuedouble;                       \
        settings.ITEM.a = json_##ITEM##_a->valuedouble;                       \
    } while (0);
