/*
  Lucy the Diamond Girl - Game where player collects diamonds.
  Copyright (C) 2005-2015  Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


  Complete license can be found in the LICENSE file.
*/

#ifndef GFX_H_
#define GFX_H_

#include "traits.h"
#include <SDL/SDL.h>
#include <stdbool.h>

#ifdef WITH_OPENGL
# include <GL/glew.h>
#endif

struct map;
struct image;
enum GAME_MODE;

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

extern bool           gfx_initialize(void);
extern void           gfx_cleanup(void);
extern void           gfx_flip(void);
extern void           gfx_2d(void);
extern void           gfx_3d(bool reset_view);
extern void           gfx_3d_depthtest(bool reset_view);
extern uint8_t *      gfx_capture_screen(void); /* Capture current screen, return SCREEN_WIDTH * SCREEN_HEIGHT * 3 rgb values. */

extern SDL_Surface *  gfx_game_screen(void); /* Return the game screen as opened with the SDL, previously this was gfx(MAP_SCREEN). */

extern void clear_area(int x, int y, int w, int h);
extern void draw_map(struct map * map, enum GAME_MODE game_mode, trait_t active_traits, int topleft_x, int topleft_y, int width, int height);
extern void draw_map_borders(struct map * map, enum GAME_MODE game_mode, trait_t active_traits, int topleft_x, int topleft_y, int width, int height);
extern void draw_legend(int x, int y);

extern void           gfx_draw_rectangle(SDL_Rect * r, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
extern void           gfx2d_draw_image(int x, int y, struct image * image);
extern void           gfx2d_draw_image_scaled(int x, int y, struct image * image, int scale_to_width, int scale_to_height);
extern void           gfx2d_draw_borders(SDL_Rect * r, uint8_t mode, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
extern void           gfx_draw_hline(int x, int y, int width, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
extern void           gfx_draw_vline(int x, int y, int height, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);

#ifdef WITH_OPENGL
extern void gfxgl_bind_texture(GLuint texture_id);        // glBindTexture()
extern void gfxgl_client_state(GLenum cap, bool enabled); // glEnableClientState() / glDisableClientState()
extern void gfxgl_state(GLenum cap, bool enabled);        // glEnable() / glDisable()
extern void gfx_colour_clear(void);
extern void gfx_colour_white(void);
extern void gfx_colour(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
#endif

#if !defined(NDEBUG) && defined(WITH_OPENGL)
extern void gfx_gl_log(char * fmt, ...);
extern void gfx_gl_error(char * fmt, ...);
#define GFX_GL_ERROR() gfx_gl_error("%s:%d > %s()", __FILE__, __LINE__, __FUNCTION__)
#else
#define gfx_gl_log(fmt, ...)
#define GFX_GL_ERROR()
#endif

#endif
