
#include <stdbool.h>
#include <GL/glew.h>

/* We implement nuklear in this file */
#define NK_IMPLEMENTATION
#define NK_SDL_GL3_IMPLEMENTATION
#include "include/menu.h"
#include "include/settings.h"

#define MENU_X 100
#define MENU_Y 100
#define MENU_W 500
#define MENU_H 300

#define MENU_FLAGS                                           \
    NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE | \
      NK_WINDOW_NO_SCROLLBAR

/*----------------------------------------------------------------------------*/

struct nk_context* nk_ctx      = NULL;
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
    nk_ctx = nk_sdl_init(window);

    /* Initialize the font */
    nk_sdl_font_stash_begin(&nk_atlas);
    nk_sdl_font_stash_end();

    /* Use our own cursors for menu */
    nk_style_load_all_cursors(nk_ctx, nk_atlas->cursors);

    return true;
}

/*----------------------------------------------------------------------------*/

void menu_render(void) {
    if (nk_begin(nk_ctx, "tf2-cheat", nk_rect(MENU_X, MENU_Y, MENU_W, MENU_H),
                 MENU_FLAGS)) {
        nk_layout_row_dynamic(nk_ctx, 20, 1);

        nk_checkbox_label(nk_ctx, "Bhop", &settings.bhop);

        nk_layout_row_dynamic(nk_ctx, 20, 2);

        static const char* autostrafe_opts[] = { "Off", "Legit", "Rage" };
        struct nk_vec2 size                  = { 70, 100 };
        nk_label(nk_ctx, "Autostrafe", NK_TEXT_LEFT);
        nk_combobox(nk_ctx, autostrafe_opts, 3, &settings.autostrafe, 15, size);

        nk_end(nk_ctx);
    }
}
