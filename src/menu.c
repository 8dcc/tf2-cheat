
#include <stdbool.h>
#include <GL/glew.h>

/* We implement nuklear in this file */
#define NK_IMPLEMENTATION
#define NK_SDL_GL3_IMPLEMENTATION
#include "include/menu.h"

#include "include/settings.h"
#include "include/globals.h"

#define MENU_FLAGS                                           \
    NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE | \
      NK_WINDOW_NO_SCROLLBAR

#define WATERMARK_FLAGS NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR

#define CHECK_TAB_COLOR(idx)                                            \
    if (idx == cur_tab) {                                               \
        ctx->style.button.normal.data.color = nk_rgba(35, 35, 35, 255); \
        ctx->style.button.hover.data.color  = nk_rgba(35, 35, 35, 255); \
    } else {                                                            \
        ctx->style.button.normal.data.color = nk_rgba(50, 50, 50, 255); \
        ctx->style.button.hover.data.color  = nk_rgba(40, 40, 40, 255); \
    }

#define ADD_TAB(idx, str)          \
    CHECK_TAB_COLOR(idx);          \
    if (nk_button_label(ctx, str)) \
        cur_tab = idx;

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

static inline void tab_movement(void) {
    nk_layout_row_dynamic(ctx, 20, 1);
    nk_checkbox_label(ctx, "Bhop", &settings.bhop);

    nk_layout_row_dynamic(ctx, 20, 2);
    static const char* autostrafe_opts[] = { "Off", "Legit", "Rage" };
    struct nk_vec2 size                  = { 70, 100 };
    nk_label(ctx, "Autostrafe", NK_TEXT_LEFT);
    nk_combobox(ctx, autostrafe_opts, 3, &settings.autostrafe, 15, size);
}

static inline void tab_esp(void) {
    nk_layout_row_dynamic(ctx, 20, 2);
    static const char* autostrafe_opts[] = { "Off", "Friendly", "Enemies",
                                             "All" };
    struct nk_vec2 size                  = { 70, 100 };
    nk_label(ctx, "Enable ESP", NK_TEXT_LEFT);
    nk_combobox(ctx, autostrafe_opts, 4, &settings.enable_esp, 15, size);

    nk_layout_row_dynamic(ctx, 20, 1);
    nk_checkbox_label(ctx, "Box ESP", &settings.box_esp);
    nk_checkbox_label(ctx, "Name ESP", &settings.name_esp);
    nk_checkbox_label(ctx, "Class ESP", &settings.class_esp);
    nk_checkbox_label(ctx, "Weapon ESP", &settings.weapon_esp);
}

static inline void tab_misc(void) {
    nk_layout_row_dynamic(ctx, 20, 1);
    nk_checkbox_label(ctx, "Watermark", &settings.watermark);
    nk_checkbox_label(ctx, "Spectator list", &settings.speclist);
    nk_checkbox_label(ctx, "Autobackstab", &settings.autostab);
}

void menu_render(void) {
    set_style();

    if (nk_begin(ctx, "Enoch", nk_rect(MENU_X, MENU_Y, MENU_W, MENU_H),
                 MENU_FLAGS)) {
        nk_layout_row_dynamic(ctx, 20, 3);

        ADD_TAB(0, "Movement");
        ADD_TAB(1, "ESP");
        ADD_TAB(2, "Misc");

        switch (cur_tab) {
            default:
            case 0:
                tab_movement();
                break;
            case 1:
                tab_esp();
                break;
            case 2:
                tab_misc();
                break;
        }
    }
    nk_end(ctx);
}

void watermark_render(void) {
    if (nk_begin(ctx, "Watermark",
                 nk_rect(WATERMARK_X, WATERMARK_Y, WATERMARK_W, WATERMARK_H),
                 WATERMARK_FLAGS)) {
        nk_layout_row_dynamic(ctx, 10, 1);
        nk_label(ctx, "8dcc/tf2-cheat", NK_TEXT_CENTERED);
    }
    nk_end(ctx);
}

void spectator_list(void) {
    if (!localplayer || !g.IsInGame)
        return;

    /* If we are dead, display spectator list for the dead guy */
    Entity* local = METHOD(localplayer, IsAlive)
                      ? localplayer
                      : METHOD(localplayer, GetObserverTarget);
    if (!local)
        return;

    int namesp = 0;
    const char* names[64];

    for (int i = 1; i <= MIN(64, g.MaxClients); i++) {
        if (i == g.localplayer)
            continue;

        Entity* ent      = METHOD_ARGS(i_entitylist, GetClientEntity, i);
        Networkable* net = GetNetworkable(ent);
        if (!ent || ent == localplayer || ent == local ||
            METHOD(net, IsDormant) || METHOD(ent, IsAlive))
            continue;

        /* Not spectating us */
        if (METHOD(ent, GetObserverMode) == OBS_MODE_NONE ||
            METHOD(ent, GetObserverTarget) != local)
            continue;

        /* Add name to list */
        player_info_t pinfo;
        if (METHOD_ARGS(i_engine, GetPlayerInfo, i, &pinfo))
            names[namesp++] = pinfo.name;
    }

    /* Create nuklear window the first time */
    if (namesp == 0)
        return;

    int scr_w = 0, scr_h = 0;
    METHOD_ARGS(i_engine, GetScreenSize, &scr_w, &scr_h);

    if (nk_begin(
          ctx, "Spectators",
          nk_rect(SPECLIST_X, scr_h / 2 - 50, SPECLIST_W, 45 + namesp * 15),
          MENU_FLAGS)) {
        nk_layout_row_dynamic(ctx, 15, 1);
        for (int i = 0; i < namesp; i++)
            nk_label(ctx, names[i], NK_TEXT_LEFT);
    }
    nk_end(ctx);
}
