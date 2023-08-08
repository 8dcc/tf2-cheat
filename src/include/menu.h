#ifndef MENU_H_
#define MENU_H_

#include <stdbool.h>

/*
 * For more information, see:
 * https://immediate-mode-ui.github.io/Nuklear/doc/index.html#nuklear/usage/flags
 * https://github.com/Immediate-Mode-UI/Nuklear/blob/614abce05b9455849bbf1519b7f86e53c78b04ab/demo/sdl_opengl3/main.c
 */
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear/nuklear.h"
#include "nuklear/nuklear_sdl_gl3.h"

/* For nk_sdl_render() */
#define MAX_VERTEX_MEMORY  512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

#define MENU_X 100
#define MENU_Y 100
#define MENU_W 300
#define MENU_H 250

#define MENU_KEY NK_KEY_INSERT

/*----------------------------------------------------------------------------*/

extern struct nk_context* ctx;
extern struct nk_font_atlas* nk_atlas;
extern SDL_GLContext orig_gl_ctx, gl_ctx;
extern bool menu_open;

/*----------------------------------------------------------------------------*/

bool menu_init(SDL_Window* window);
void menu_render(void);

#endif /* MENU_H_ */
