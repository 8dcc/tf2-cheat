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

/* Defined at the bottom of this file */
#define MAX_CFGS     30
#define MAX_CFG_NAME 200
static inline int fill_configs(char* config_list[MAX_CFGS]);
static inline void free_configs(char* config_list[MAX_CFGS], int config_num);

#define COLOR_PICKER_H 120
#define COMBOBOX_H     17
#define COMBO_DROP_W   ((MENU_W - 21) / 2 - 3)

#define RESET_BUTTON_COLOR()                                               \
    do {                                                                   \
        ctx->style.button.normal.data.color = nk_rgba(50, 50, 50, 255);    \
        ctx->style.button.hover.data.color  = nk_rgba(40, 40, 40, 255);    \
        ctx->style.button.text_normal       = nk_rgba(175, 175, 175, 255); \
        ctx->style.button.text_hover        = nk_rgba(175, 175, 175, 255); \
        ctx->style.button.text_active       = nk_rgba(175, 175, 175, 255); \
    } while (0)

#define SET_BUTTON_TEXT_COLOR(col)           \
    do {                                     \
        ctx->style.button.text_normal = col; \
        ctx->style.button.text_hover  = col; \
        ctx->style.button.text_active = col; \
    } while (0)

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

#define SLIDER_FLOAT(STRING, MIN, SETTING, MAX, STEP)                   \
    do {                                                                \
        char* tmpPtr = malloc(strlen(STRING) + sizeof(" (999999.99)")); \
        sprintf(tmpPtr, "%s (%.2f)", STRING, SETTING);                  \
        nk_layout_row_dynamic(ctx, 15, 2);                              \
        nk_label(ctx, tmpPtr, NK_TEXT_LEFT);                            \
        nk_slider_float(ctx, MIN, &SETTING, MAX, STEP);                 \
        free(tmpPtr);                                                   \
    } while (0)

#define SLIDER_INT(STRING, MIN, SETTING, MAX, STEP)                  \
    do {                                                             \
        char* tmpPtr = malloc(strlen(STRING) + sizeof(" (999999)")); \
        sprintf(tmpPtr, "%s (%d)", STRING, SETTING);                 \
        nk_layout_row_dynamic(ctx, 15, 2);                           \
        nk_label(ctx, tmpPtr, NK_TEXT_LEFT);                         \
        nk_slider_int(ctx, MIN, &SETTING, MAX, STEP);                \
        free(tmpPtr);                                                \
    } while (0)

#define COLOR_PICKER(SETTING) SETTING = nk_color_picker(ctx, SETTING, NK_RGBA)

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

static inline void tab_esp(void) {
    nk_layout_row_dynamic(ctx, COMBOBOX_H, 2);
    static const char* opts0[] = { "Off", "Teammates", "Enemies", "All" };
    struct nk_vec2 size0       = { COMBO_DROP_W, 200 };
    nk_label(ctx, "Player ESP", NK_TEXT_LEFT);
    nk_combobox(ctx, opts0, 4, &settings.esp_player, 15, size0);

    nk_layout_row_dynamic(ctx, 15, 1);
    nk_checkbox_label(ctx, "Use team color", &settings.esp_use_team_color);
    nk_checkbox_label(ctx, "Player box", &settings.esp_player_box);
    nk_checkbox_label(ctx, "Player skeleton", &settings.esp_skeleton);
    nk_checkbox_label(ctx, "Player health", &settings.esp_player_health);
    nk_checkbox_label(ctx, "Player name", &settings.esp_player_name);
    nk_checkbox_label(ctx, "Player class", &settings.esp_player_class);
    nk_checkbox_label(ctx, "Player weapon", &settings.esp_player_weapon);
    nk_checkbox_label(ctx, "Player conditions", &settings.esp_player_cond);
    nk_checkbox_label(ctx, "Ignore invisible", &settings.esp_ignore_invisible);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */
    nk_layout_row_dynamic(ctx, COMBOBOX_H, 2);

    static const char* opts1[] = { "Off", "Teammates", "Enemies", "All" };
    struct nk_vec2 size1       = { COMBO_DROP_W, 200 };
    nk_label(ctx, "Building ESP", NK_TEXT_LEFT);
    nk_combobox(ctx, opts1, 4, &settings.esp_building, 15, size1);

    static const char* opts2[] = { "All", "Sentries", "Dispensers",
                                   "Teleporters" };
    struct nk_vec2 size2       = { COMBO_DROP_W, 200 };
    nk_label(ctx, "Building types", NK_TEXT_LEFT);
    nk_combobox(ctx, opts2, 4, &settings.esp_building_type, 15, size2);

    nk_layout_row_dynamic(ctx, 15, 1);
    nk_checkbox_label(ctx, "Use team color",
                      &settings.esp_building_use_team_color);
    nk_checkbox_label(ctx, "Building box", &settings.esp_building_box);
    nk_checkbox_label(ctx, "Building health", &settings.esp_building_hp);
    nk_checkbox_label(ctx, "Building name", &settings.esp_building_name);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */
    nk_layout_row_dynamic(ctx, COMBOBOX_H, 2);

    static const char* opts3[] = { "Off", "Teammates", "Enemies", "All" };
    struct nk_vec2 size3       = { COMBO_DROP_W, 200 };
    nk_label(ctx, "Stickybomb ESP", NK_TEXT_LEFT);
    nk_combobox(ctx, opts3, 4, &settings.esp_sticky, 15, size3);
    nk_checkbox_label(ctx, "Use team color",
                      &settings.esp_sticky_use_team_color);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */
    nk_layout_row_dynamic(ctx, 15, 1);

    nk_checkbox_label(ctx, "Ammo box ESP", &settings.esp_ammobox);
    nk_checkbox_label(ctx, "Healing items ESP", &settings.esp_healthpack);
}

static inline void tab_aim(void) {
    nk_layout_row_dynamic(ctx, 15, 1);
    nk_checkbox_label(ctx, "Target steam friends",
                      &settings.aim_target_steam_friends);
    nk_checkbox_label(ctx, "Target invisible", &settings.aim_target_invisible);
    nk_checkbox_label(ctx, "Target invulnerable", &settings.aim_target_invul);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */
    nk_layout_row_dynamic(ctx, 15, 1);

    nk_checkbox_label(ctx, "Aimbot", &settings.aimbot);

    SLIDER_FLOAT("Aimbot FOV", 0.f, settings.aim_fov, 180.f, 0.5f);
    SLIDER_FLOAT("Smoothing amount", 1.f, settings.aim_smooth, 50.f, 0.5f);
    SLIDER_FLOAT("Degree threshold", 0.f, settings.aim_deg_threshold, 5.f,
                 0.1f);

    nk_layout_row_dynamic(ctx, COMBOBOX_H, 2);
    static const char* opts0[] = { "Head", "Torso", "Arms", "Legs" };
    struct nk_vec2 size0       = { COMBO_DROP_W, 200 };
    nk_label(ctx, "Aimbot hitboxes", NK_TEXT_LEFT);
    nk_combobox(ctx, opts0, 4, &settings.aim_hitbox, 15, size0);

    nk_layout_row_dynamic(ctx, 15, 1);
    nk_checkbox_label(ctx, "Silent", &settings.aim_silent);
    nk_checkbox_label(ctx, "On key", &settings.aim_on_key);
    nk_checkbox_label(ctx, "Ignore walls", &settings.aim_ignore_walls);
    nk_checkbox_label(ctx, "Auto-scope", &settings.aim_autoscope);
    nk_checkbox_label(ctx, "Disable if unscoped", &settings.aim_off_unscoped);
    nk_checkbox_label(ctx, "Disable if spectated", &settings.aim_off_spectated);
    nk_checkbox_label(ctx, "Draw FOV circle", &settings.aim_draw_fov);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */
    nk_layout_row_dynamic(ctx, 15, 1);

    nk_checkbox_label(ctx, "Meleebot", &settings.meleebot);
    nk_checkbox_label(ctx, "pSilent", &settings.melee_silent);
    nk_checkbox_label(ctx, "Swing prediction", &settings.melee_swing_pred);
    nk_checkbox_label(ctx, "On key", &settings.melee_on_key);
    nk_checkbox_label(ctx, "Disable if spectated",
                      &settings.melee_off_spectated);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */
    nk_layout_row_dynamic(ctx, 15, 1);

    nk_checkbox_label(ctx, "Automedigun", &settings.automedigun);
    nk_checkbox_label(ctx, "Silent", &settings.automedigun_silent);
    SLIDER_FLOAT("Medigun smoothing", 1.f, settings.automedigun_smooth, 100.f,
                 0.5f);
    SLIDER_FLOAT("Target switch time", 0.f, settings.automedigun_switch_time,
                 5.f, 0.1f);
}

static inline void tab_visuals(void) {
    nk_layout_row_dynamic(ctx, COMBOBOX_H, 2);
    static const char* opts1[] = { "Off", "Teammates", "Enemies", "All" };
    struct nk_vec2 size1       = { COMBO_DROP_W, 200 };
    nk_label(ctx, "Player chams", NK_TEXT_LEFT);
    nk_combobox(ctx, opts1, 4, &settings.chams_player, 15, size1);

    nk_layout_row_dynamic(ctx, 15, 1);
    nk_checkbox_label(ctx, "Use team color",
                      &settings.chams_player_use_team_color);
    nk_checkbox_label(ctx, "Ignore Z", &settings.chams_ignorez);
    nk_checkbox_label(ctx, "Thirdperson chams", &settings.chams_local);
    nk_checkbox_label(ctx, "Weapon chams", &settings.chams_weapon);
    nk_checkbox_label(ctx, "Hand chams", &settings.chams_hand);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */
    nk_layout_row_dynamic(ctx, 15, 1);

    nk_checkbox_label(ctx, "Remove scope overlay", &settings.remove_scope);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */
    nk_layout_row_dynamic(ctx, 15, 1);

    nk_checkbox_label(ctx, "Custom FOV", &settings.custom_fov);
    SLIDER_FLOAT("Degrees", 60.f, settings.custom_fov_deg, 130.f, 2.f);
    nk_checkbox_label(ctx, "Apply when scoped", &settings.custom_fov_scoped);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */
    nk_layout_row_dynamic(ctx, 15, 1);

    nk_checkbox_label(ctx, "Thirdperson", &settings.thirdperson);
    nk_checkbox_label(ctx, "Use network angles", &settings.thirdperson_network);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */
    nk_layout_row_dynamic(ctx, 15, 1);

    nk_checkbox_label(ctx, "Show velocity", &settings.draw_velocity);
    nk_checkbox_label(ctx, "Show jump velocity", &settings.draw_velocity_jump);
    nk_checkbox_label(ctx, "Show vertical velocity",
                      &settings.draw_velocity_vert);
    SLIDER_FLOAT("Position", 0.f, settings.draw_velocity_pos, 100.f, 0.1f);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */
    nk_layout_row_dynamic(ctx, 15, 1);

    nk_checkbox_label(ctx, "Watermark", &settings.watermark);
    nk_checkbox_label(ctx, "Spectator list", &settings.speclist);
    SLIDER_FLOAT("Spectator list height", 0.f, settings.speclist_height, 100.f,
                 0.1f);
}

static inline void tab_misc(void) {
    nk_layout_row_dynamic(ctx, 15, 1);
    nk_checkbox_label(ctx, "Bhop", &settings.bhop);

    nk_layout_row_dynamic(ctx, COMBOBOX_H, 2);
    static const char* opts0[] = { "Off", "Legit", "Rage" };
    struct nk_vec2 size0       = { COMBO_DROP_W, 200 };
    nk_label(ctx, "Autostrafe", NK_TEXT_LEFT);
    nk_combobox(ctx, opts0, 3, &settings.autostrafe, 15, size0);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */
    nk_layout_row_dynamic(ctx, 15, 1);

    nk_checkbox_label(ctx, "Anti-aim", &settings.aa);
    SLIDER_FLOAT("Pitch", -89.f, settings.aa_pitch, 89.f, 1.f);
    SLIDER_FLOAT("Yaw", -180.f, settings.aa_yaw, 180.f, 2.f);
    nk_checkbox_label(ctx, "Spinbot", &settings.aa_spin);
    SLIDER_FLOAT("Spinbot speed", 0.f, settings.aa_speed, 100.f, 0.5f);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */
    nk_layout_row_dynamic(ctx, 15, 1);

    nk_checkbox_label(ctx, "Autorocketjump (mouse2)", &settings.rocketjump);
    SLIDER_FLOAT("Degrees when moving", 25.f, settings.rocketjump_deg, 89.f,
                 0.5f);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */
    nk_layout_row_dynamic(ctx, 15, 1);

    nk_checkbox_label(ctx, "Auto-detonate stickybombs",
                      &settings.auto_detonate);
    nk_checkbox_label(ctx, "Apply to local player",
                      &settings.auto_detonate_self);
    SLIDER_FLOAT("Target distance", 20.f, settings.auto_detonate_dist, 180.f,
                 1.f);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */
    nk_layout_row_dynamic(ctx, 15, 1);

    nk_checkbox_label(ctx, "Melee crithack", &settings.crits_melee);
    SLIDER_INT("Crithack chance", 1, settings.crits_chance, 100, 1);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */
    nk_layout_row_dynamic(ctx, 15, 1);

    nk_checkbox_label(ctx, "Autobackstab", &settings.autostab);
    nk_checkbox_label(ctx, "Only check animation", &settings.anim_stab);

    nk_layout_row_dynamic(ctx, 8, 1);
    nk_spacing(ctx, 0); /* ----------------------------  */
    nk_layout_row_dynamic(ctx, 15, 1);

    nk_checkbox_label(ctx, "NoPush", &settings.nopush);
    nk_checkbox_label(ctx, "Anti-AFK", &settings.antiafk);
    nk_checkbox_label(ctx, "Hide cheat in screenshots",
                      &settings.clean_screenshots);
}

static inline void tab_colors(void) {
    /* Team colors */
    nk_layout_row_dynamic(ctx, 15, 2);
    nk_label(ctx, "RED team", NK_TEXT_CENTERED);
    nk_label(ctx, "BLU team", NK_TEXT_CENTERED);
    nk_layout_row_dynamic(ctx, COLOR_PICKER_H, 2);
    COLOR_PICKER(settings.col_team_red);
    COLOR_PICKER(settings.col_team_blu);

    /* Esp colors */
    nk_layout_row_dynamic(ctx, 15, 2);
    nk_label(ctx, "Teammate player ESP", NK_TEXT_CENTERED);
    nk_label(ctx, "Enemy player ESP", NK_TEXT_CENTERED);
    nk_layout_row_dynamic(ctx, COLOR_PICKER_H, 2);
    COLOR_PICKER(settings.col_esp_tmate);
    COLOR_PICKER(settings.col_esp_enemy);

    nk_layout_row_dynamic(ctx, 15, 2);
    nk_label(ctx, "Teammate building ESP", NK_TEXT_CENTERED);
    nk_label(ctx, "Enemy building ESP", NK_TEXT_CENTERED);
    nk_layout_row_dynamic(ctx, COLOR_PICKER_H, 2);
    COLOR_PICKER(settings.col_esp_build_tmate);
    COLOR_PICKER(settings.col_esp_build_enemy);

    nk_layout_row_dynamic(ctx, 15, 2);
    nk_label(ctx, "Teammate sticky ESP", NK_TEXT_CENTERED);
    nk_label(ctx, "Enemy sticky ESP", NK_TEXT_CENTERED);
    nk_layout_row_dynamic(ctx, COLOR_PICKER_H, 2);
    COLOR_PICKER(settings.col_esp_sticky_tmate);
    COLOR_PICKER(settings.col_esp_sticky_enemy);

    nk_layout_row_dynamic(ctx, 15, 2);
    nk_label(ctx, "Ammo boxes ESP", NK_TEXT_CENTERED);
    nk_label(ctx, "Healing items ESP", NK_TEXT_CENTERED);
    nk_layout_row_dynamic(ctx, COLOR_PICKER_H, 2);
    COLOR_PICKER(settings.col_esp_ammobox);
    COLOR_PICKER(settings.col_esp_healthpack);

    nk_layout_row_dynamic(ctx, 15, 2);
    nk_label(ctx, "Steam friend ESP", NK_TEXT_CENTERED);
    nk_label(ctx, "Player condition ESP", NK_TEXT_CENTERED);
    nk_layout_row_dynamic(ctx, COLOR_PICKER_H, 2);
    COLOR_PICKER(settings.col_esp_steam_friend);
    COLOR_PICKER(settings.col_esp_player_cond);

    /* Chams colors */
    nk_layout_row_dynamic(ctx, 15, 2);
    nk_label(ctx, "Teammate player chams", NK_TEXT_CENTERED);
    nk_label(ctx, "Enemy player chams", NK_TEXT_CENTERED);
    nk_layout_row_dynamic(ctx, COLOR_PICKER_H, 2);
    COLOR_PICKER(settings.col_chams_tmate);
    COLOR_PICKER(settings.col_chams_enemy);

    /* Misc colors */
    nk_layout_row_dynamic(ctx, 15, 2);
    nk_label(ctx, "Thirdperson chams", NK_TEXT_CENTERED);
    nk_label(ctx, "Weapon chams", NK_TEXT_CENTERED);
    nk_layout_row_dynamic(ctx, COLOR_PICKER_H, 2);
    COLOR_PICKER(settings.col_chams_local);
    COLOR_PICKER(settings.col_chams_weapon);

    nk_layout_row_dynamic(ctx, 15, 2);
    nk_label(ctx, "Hand chams", NK_TEXT_CENTERED);
    nk_label(ctx, "Aimbot FOV circle", NK_TEXT_CENTERED);
    nk_layout_row_dynamic(ctx, COLOR_PICKER_H, 2);
    COLOR_PICKER(settings.col_chams_hand);
    COLOR_PICKER(settings.col_aim_fov);
}

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

    nk_layout_row_dynamic(ctx, 12, 1);
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

    /* User selected last item, then deleted file, reset selected number */
    if (selected_config >= config_num)
        selected_config = 0;

    nk_layout_row_dynamic(ctx, 22, 2);

    if (nk_button_label(ctx, "Load"))
        load_config(config_list[selected_config]);

    SET_BUTTON_TEXT_COLOR(nk_rgba(185, 34, 34, 255));
    if (nk_button_label(ctx, "Save (Overwrite)"))
        save_config(config_list[selected_config]);
    RESET_BUTTON_COLOR();

    free_configs(config_list, config_num);
}

void menu_render(void) {
    set_style();

    if (nk_begin(ctx, "Enoch", nk_rect(MENU_X, MENU_Y, MENU_W, MENU_H),
                 MENU_FLAGS)) {
        nk_layout_row_dynamic(ctx, 20, 6);

        ADD_TAB(0, "ESP");
        ADD_TAB(1, "Aim");
        ADD_TAB(2, "Visuals");
        ADD_TAB(3, "Misc");
        ADD_TAB(4, "Colors");
        ADD_TAB(5, "Config");

        nk_layout_row_dynamic(ctx, 5, 1);
        nk_spacing(ctx, 0); /* ----------------------------  */

        switch (cur_tab) {
            default:
            case 0:
                tab_esp();
                break;
            case 1:
                tab_aim();
                break;
            case 2:
                tab_visuals();
                break;
            case 3:
                tab_misc();
                break;
            case 4:
                tab_colors();
                break;
            case 5:
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

/*----------------------------------------------------------------------------*/

static inline int fill_configs(char* config_list[MAX_CFGS]) {
    DIR* d = opendir(CONFIG_FOLDER);
    if (!d)
        return 0;

    int i = 0;
    struct dirent* dir;
    while (i < MAX_CFGS && (dir = readdir(d)) != NULL) {
        if (dir->d_type == DT_REG) {
            config_list[i] = calloc(strlen(dir->d_name) + 1, sizeof(char));
            strcpy(config_list[i], dir->d_name);
            i++;
        }
    }

    closedir(d);

    return i >= MAX_CFGS ? i - 1 : i;
}

static inline void free_configs(char* config_list[MAX_CFGS], int config_num) {
    for (int i = 0; i < config_num; i++)
        if (config_list[i])
            free(config_list[i]);
}
