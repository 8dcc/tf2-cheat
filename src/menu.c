
#include <stdbool.h>
#include <GL/glew.h>

/* We implement nuklear in this file */
#define NK_IMPLEMENTATION
#define NK_SDL_GL3_IMPLEMENTATION
#include "include/menu.h"
#include "include/settings.h"

#define MENU_X 100
#define MENU_Y 100
#define MENU_W 300
#define MENU_H 250

#define MENU_FLAGS                                           \
    NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE | \
      NK_WINDOW_NO_SCROLLBAR

/*----------------------------------------------------------------------------*/

struct nk_context* ctx         = NULL;
struct nk_font_atlas* nk_atlas = NULL;
SDL_GLContext orig_gl_ctx, gl_ctx;
bool menu_open = false;

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

    /* Use our own cursors for menu */
    nk_style_load_all_cursors(ctx, nk_atlas->cursors);

    return true;
}

/*----------------------------------------------------------------------------*/

static void set_style(struct nk_context* ctx) {
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
    table[NK_COLOR_TOGGLE_CURSOR]           = nk_rgba(107, 118, 255, 255);
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
}

void menu_render(void) {
    set_style(ctx);

    if (nk_begin(ctx, "tf2-cheat", nk_rect(MENU_X, MENU_Y, MENU_W, MENU_H),
                 MENU_FLAGS)) {
        nk_layout_row_dynamic(ctx, 20, 1);

        nk_checkbox_label(ctx, "Bhop", &settings.bhop);

        nk_layout_row_dynamic(ctx, 20, 2);

        static const char* autostrafe_opts[] = { "Off", "Legit", "Rage" };
        struct nk_vec2 size                  = { 70, 100 };
        nk_label(ctx, "Autostrafe", NK_TEXT_LEFT);
        nk_combobox(ctx, autostrafe_opts, 3, &settings.autostrafe, 15, size);

        nk_end(ctx);
    }
}
