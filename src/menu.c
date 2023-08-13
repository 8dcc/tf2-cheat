#include <stdbool.h>
#include <dirent.h>
#include <GL/glew.h>

/* We implement nuklear in this file */
#define NK_IMPLEMENTATION
#define NK_SDL_GL3_IMPLEMENTATION
#include "include/menu.h"

#include "include/settings.h"

#define MENU_FLAGS      NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE
#define WATERMARK_FLAGS NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR

#define RESET_BUTTON_COLOR()                                        \
    ctx->style.button.normal.data.color = nk_rgba(50, 50, 50, 255); \
    ctx->style.button.hover.data.color  = nk_rgba(40, 40, 40, 255);

#define CHECK_TAB_COLOR(idx)                                            \
    if (idx == cur_tab) {                                               \
        ctx->style.button.normal.data.color = nk_rgba(35, 35, 35, 255); \
        ctx->style.button.hover.data.color  = nk_rgba(35, 35, 35, 255); \
    } else {                                                            \
        RESET_BUTTON_COLOR();                                           \
    }

#define ADD_TAB(idx, str)          \
    CHECK_TAB_COLOR(idx);          \
    if (nk_button_label(ctx, str)) \
        cur_tab = idx;             \
    RESET_BUTTON_COLOR();

/*----------------------------------------------------------------------------*/

struct nk_context* ctx         = NULL;
struct nk_font_atlas* nk_atlas = NULL;
SDL_GLContext orig_gl_ctx, gl_ctx;
bool menu_open = false;

/* Static globals */
static int cur_tab = 0;

/*----------------------------------------------------------------------------*/

bool menu_init(SDL_Window* window) {
    /* Old and new SDL contexts */
    orig_gl_ctx = SDL_GL_GetCurrentContext();
    gl_ctx      = SDL_GL_CreateContext(window);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "menu_init: glewInit error: %s\n",
                glewGetErrorString(err));
        return false;
    }

    /* New nuklear context. Global variable */
    ctx = nk_sdl_init(window);

    /* Initialize the font */
    nk_sdl_font_stash_begin(&nk_atlas);
    nk_sdl_font_stash_end();

    return true;
}

/*----------------------------------------------------------------------------*/

static void set_style(void) {
    /* Colors */
    struct nk_color table[NK_COLOR_COUNT];
    table[NK_COLOR_TEXT]                    = nk_rgba(175, 175, 175, 255);
    table[NK_COLOR_WINDOW]                  = nk_rgba(45, 45, 45, 255);
    table[NK_COLOR_HEADER]                  = nk_rgba(40, 40, 40, 255);
    table[NK_COLOR_BORDER]                  = nk_rgba(65, 65, 65, 255);
    table[NK_COLOR_BUTTON]                  = nk_rgba(50, 50, 50, 255);
    table[NK_COLOR_BUTTON_HOVER]            = nk_rgba(40, 40, 40, 255);
    table[NK_COLOR_BUTTON_ACTIVE]           = nk_rgba(35, 35, 35, 255);
    table[NK_COLOR_TOGGLE]                  = nk_rgba(70, 70, 70, 255);
    table[NK_COLOR_TOGGLE_HOVER]            = nk_rgba(80, 80, 80, 255);
    table[NK_COLOR_TOGGLE_CURSOR]           = nk_rgba(127, 154, 176, 255); /**/
    table[NK_COLOR_SELECT]                  = nk_rgba(45, 45, 45, 255);
    table[NK_COLOR_SELECT_ACTIVE]           = nk_rgba(35, 35, 35, 255);
    table[NK_COLOR_SLIDER]                  = nk_rgba(38, 38, 38, 255);
    table[NK_COLOR_SLIDER_CURSOR]           = nk_rgba(100, 100, 100, 255);
    table[NK_COLOR_SLIDER_CURSOR_HOVER]     = nk_rgba(120, 120, 120, 255);
    table[NK_COLOR_SLIDER_CURSOR_ACTIVE]    = nk_rgba(150, 150, 150, 255);
    table[NK_COLOR_PROPERTY]                = nk_rgba(38, 38, 38, 255);
    table[NK_COLOR_EDIT]                    = nk_rgba(38, 38, 38, 255);
    table[NK_COLOR_EDIT_CURSOR]             = nk_rgba(175, 175, 175, 255);
    table[NK_COLOR_COMBO]                   = nk_rgba(45, 45, 45, 255);
    table[NK_COLOR_CHART]                   = nk_rgba(120, 120, 120, 255);
    table[NK_COLOR_CHART_COLOR]             = nk_rgba(45, 45, 45, 255);
    table[NK_COLOR_CHART_COLOR_HIGHLIGHT]   = nk_rgba(255, 0, 0, 255);
    table[NK_COLOR_SCROLLBAR]               = nk_rgba(40, 40, 40, 255);
    table[NK_COLOR_SCROLLBAR_CURSOR]        = nk_rgba(100, 100, 100, 255);
    table[NK_COLOR_SCROLLBAR_CURSOR_HOVER]  = nk_rgba(120, 120, 120, 255);
    table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(150, 150, 150, 255);
    table[NK_COLOR_TAB_HEADER]              = nk_rgba(40, 40, 40, 255);
    nk_style_from_table(ctx, table);

    /* Misc */
    ctx->style.button.rounding         = 0.f;
    ctx->style.checkbox.padding.x      = 2.f;
    ctx->style.checkbox.padding.y      = 2.f;
    ctx->style.combo.rounding          = 0.f;
    ctx->style.combo.content_padding.x = 7.f;
    ctx->style.combo.content_padding.y = 7.f;
}

/*----------------------------------------------------------------------------*/

#define COMBO_DROP_W 161

static inline void tab_esp(void) {
    nk_layout_row_dynamic(ctx, 18, 2);
    static const char* opts0[] = { "Off", "Friendly", "Enemies", "All" };
    struct nk_vec2 size0       = { COMBO_DROP_W, 200 };
    nk_label(ctx, "Player ESP", NK_TEXT_LEFT);
    nk_combobox(ctx, opts0, 4, &settings.player_esp, 15, size0);

    nk_layout_row_dynamic(ctx, 15, 1);
    nk_checkbox_label(ctx, "Player box", &settings.player_box_esp);
    nk_checkbox_label(ctx, "Player skeleton", &settings.skeleton_esp);
    nk_checkbox_label(ctx, "Player health", &settings.player_health_esp);
    nk_checkbox_label(ctx, "Player name", &settings.player_name_esp);
    nk_checkbox_label(ctx, "Player class", &settings.player_class_esp);
    nk_checkbox_label(ctx, "Player weapon", &settings.player_weapon_esp);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */
    nk_layout_row_dynamic(ctx, 18, 2);

    static const char* opts1[] = { "Off", "Friendly", "Enemies", "All" };
    struct nk_vec2 size1       = { COMBO_DROP_W, 200 };
    nk_label(ctx, "Building ESP", NK_TEXT_LEFT);
    nk_combobox(ctx, opts1, 4, &settings.building_esp, 15, size1);

    static const char* opts2[] = { "All", "Sentries", "Dispensers",
                                   "Teleporters" };
    struct nk_vec2 size2       = { COMBO_DROP_W, 200 };
    nk_label(ctx, "Building types", NK_TEXT_LEFT);
    nk_combobox(ctx, opts2, 4, &settings.building_esp_type, 15, size2);

    nk_layout_row_dynamic(ctx, 15, 1);
    nk_checkbox_label(ctx, "Building box", &settings.building_box_esp);
    nk_checkbox_label(ctx, "Building health", &settings.building_hp_esp);
    nk_checkbox_label(ctx, "Building name", &settings.building_name_esp);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */
    nk_layout_row_dynamic(ctx, 15, 1);

    nk_checkbox_label(ctx, "Ammo box ESP", &settings.ammobox_esp);
    nk_checkbox_label(ctx, "Healing items ESP", &settings.healthpack_esp);
}

static inline void tab_misc(void) {
    nk_layout_row_dynamic(ctx, 15, 1);
    nk_checkbox_label(ctx, "Bhop", &settings.bhop);

    nk_layout_row_dynamic(ctx, 18, 2);
    static const char* opts0[] = { "Off", "Legit", "Rage" };
    struct nk_vec2 size0       = { COMBO_DROP_W, 200 };
    nk_label(ctx, "Autostrafe", NK_TEXT_LEFT);
    nk_combobox(ctx, opts0, 3, &settings.autostrafe, 15, size0);

    nk_layout_row_dynamic(ctx, 15, 1);
    nk_checkbox_label(ctx, "Autobackstab", &settings.autostab);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */
    nk_layout_row_dynamic(ctx, 15, 1);

    nk_checkbox_label(ctx, "Watermark", &settings.watermark);
    nk_checkbox_label(ctx, "Spectator list", &settings.speclist);
}

static inline void tab_colors(void) {
    nk_layout_row_dynamic(ctx, 15, 2);
    nk_label(ctx, "Friendly player", NK_TEXT_CENTERED);
    nk_label(ctx, "Enemy player", NK_TEXT_CENTERED);
    nk_layout_row_dynamic(ctx, 100, 2);
    settings.col_friend_esp =
      nk_color_picker(ctx, settings.col_friend_esp, NK_RGBA);
    settings.col_enemy_esp =
      nk_color_picker(ctx, settings.col_enemy_esp, NK_RGBA);

    nk_layout_row_dynamic(ctx, 15, 2);
    nk_label(ctx, "Friendly building", NK_TEXT_CENTERED);
    nk_label(ctx, "Enemy building", NK_TEXT_CENTERED);
    nk_layout_row_dynamic(ctx, 100, 2);
    settings.col_friend_build =
      nk_color_picker(ctx, settings.col_friend_build, NK_RGBA);
    settings.col_enemy_build =
      nk_color_picker(ctx, settings.col_enemy_build, NK_RGBA);

    nk_layout_row_dynamic(ctx, 15, 2);
    nk_label(ctx, "Ammo boxes", NK_TEXT_CENTERED);
    nk_label(ctx, "Healing items", NK_TEXT_CENTERED);
    nk_layout_row_dynamic(ctx, 100, 2);
    settings.col_ammobox_esp =
      nk_color_picker(ctx, settings.col_ammobox_esp, NK_RGBA);
    settings.col_healthpack_esp =
      nk_color_picker(ctx, settings.col_healthpack_esp, NK_RGBA);
}

#define MAX_CFGS 30

static inline int fill_configs(char* config_list[MAX_CFGS]) {
    DIR* d = opendir(CONFIG_FOLDER);
    if (!d)
        return 0;

    int i = 0;
    struct dirent* dir;
    while (i < MAX_CFGS && (dir = readdir(d)) != NULL) {
        if (dir->d_type == DT_REG) {
            config_list[i] = calloc(strlen(dir->d_name), sizeof(char));
            strcpy(config_list[i], dir->d_name);
            i++;
        }
    }

    closedir(d);

    return i >= MAX_CFGS ? i - 1 : i;
}

static inline void free_configs(char* config_list[MAX_CFGS], int config_num) {
    for (int i = 0; i < config_num; i++)
        free(config_list[i]);
}

#define MAX_CFG_NAME 100

static inline void tab_config(void) {
    static char cfg_name[MAX_CFG_NAME] = { '\0' };
    static char* config_list[MAX_CFGS];
    static int selected_config = 0;

    nk_layout_row_dynamic(ctx, 15, 1);
    nk_label(ctx, "New config:", NK_TEXT_LEFT);

    nk_layout_row_dynamic(ctx, 25, 1);
    nk_edit_string_zero_terminated(ctx, NK_EDIT_BOX, cfg_name, sizeof(cfg_name),
                                   nk_filter_ascii);

    nk_layout_row_dynamic(ctx, 22, 1);
    if (nk_button_label(ctx, "Create and save"))
        save_config(cfg_name);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */

    /* Get config list from dir and display them */
    int config_num = fill_configs(config_list);
    if (config_num == 0)
        return;

    nk_layout_row_dynamic(ctx, 15, 1);
    nk_label(ctx, "Existing config:", NK_TEXT_LEFT);

    nk_layout_row_dynamic(ctx, 22, 1);
    struct nk_vec2 combo_size = { MENU_W - 23, 200 };
    nk_combobox(ctx, (const char**)config_list, config_num, &selected_config,
                15, combo_size);

    nk_layout_row_dynamic(ctx, 22, 2);
    if (nk_button_label(ctx, "Save"))
        save_config(config_list[selected_config]);
    if (nk_button_label(ctx, "Load"))
        load_config(config_list[selected_config]);

    free_configs(config_list, config_num);
}

void menu_render(void) {
    set_style();

    if (nk_begin(ctx, "Enoch", nk_rect(MENU_X, MENU_Y, MENU_W, MENU_H),
                 MENU_FLAGS)) {
        nk_layout_row_dynamic(ctx, 20, 4);

        ADD_TAB(0, "ESP");
        ADD_TAB(1, "Misc");
        ADD_TAB(2, "Colors");
        ADD_TAB(3, "Config");

        nk_layout_row_dynamic(ctx, 5, 1);
        nk_spacing(ctx, 0); /* ----------------------------  */

        switch (cur_tab) {
            default:
            case 0:
                tab_esp();
                break;
            case 1:
                tab_misc();
                break;
            case 2:
                tab_colors();
                break;
            case 3:
                tab_config();
                break;
        }
    }
    nk_end(ctx);
}

/*----------------------------------------------------------------------------*/

void watermark_render(void) {
    if (nk_begin(ctx, "Watermark",
                 nk_rect(WATERMARK_X, WATERMARK_Y, WATERMARK_W, WATERMARK_H),
                 WATERMARK_FLAGS)) {
        nk_layout_row_dynamic(ctx, 10, 1);
        nk_label(ctx, "8dcc/tf2-cheat", NK_TEXT_CENTERED);
    }
    nk_end(ctx);
}
