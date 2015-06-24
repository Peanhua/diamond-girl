/*
  Diamond Girl - Game where player collects diamonds.
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

#ifndef GFX_GLYPH_H_
#define GFX_GLYPH_H_

#include <stdbool.h>
#ifdef WITH_OPENGL
#include <GL/glew.h>
#endif

enum MAP_GLYPH;
enum MOVE_DIRECTION;
struct gfxbuf;

extern bool gfx_glyph_initialize(void);
extern void gfx_glyph_cleanup(void);

extern void    gfx_frame0(void);
extern void    gfx_next_frame(void);
extern uint8_t gfx_get_frame(enum MAP_GLYPH glyph); /* Return the current frame of a glyph. */
extern uint8_t gfx_get_maxframe(enum MAP_GLYPH glyph); /* Return the max frame of a glyph. */
extern void    gfx_set_current_glyph_set(int glyph_set);

/* The render and draw functions might have optimized inputs combining the glyph id and facing together into 16 bit value,
 * this is done to minimize the number of arguments passed.
 * In some ARM CPUs four parameters are passed in registers, rest are passed in stack.
 * http://shervinemami.info/armAssembly.html#cortex-a9
 */
#ifdef __arm__
# define GFX_GLYPH_OPTIMIZE_PARAMETERS
#else
# undef  GFX_GLYPH_OPTIMIZE_PARAMETERS
#endif

#ifdef WITH_OPENGL
# ifdef GFX_GLYPH_OPTIMIZE_PARAMETERS
#define gfx_glyph_render(gfxbuf, x, y, glyph, facing)              gfx_glyph_frender(gfxbuf, x, y, (facing << 8) | glyph)
#define gfx_glyph_render_frame(gfxbuf, x, y, glyph, facing, frame) gfx_glyph_frender_frame(gfxbuf, x, y, (facing << 8) | glyph, frame)
extern void gfx_glyph_frender(struct gfxbuf * restrict gfxbuf, int x, int y, uint16_t glyph_facing);
extern void gfx_glyph_frender_frame(struct gfxbuf * restrict gfxbuf, int x, int y, uint16_t glyph_facing, int frame);
# else
extern void gfx_glyph_render(struct gfxbuf * restrict gfxbuf, int x, int y, enum MAP_GLYPH glyph, enum MOVE_DIRECTION facing);
extern void gfx_glyph_render_frame(struct gfxbuf * restrict gfxbuf, int x, int y, enum MAP_GLYPH glyph, enum MOVE_DIRECTION facing, int frame);
# endif
#endif

#ifdef WITH_NONOPENGL
# ifdef GFX_GLYPH_OPTIMIZE_PARAMETERS
#define gfx_draw_glyph(x, y, glyph, facing)              gfx_fdraw_glyph(x, y, (facing << 8) | glyph)
#define gfx_draw_glyph_frame(x, y, glyph, facing, frame) gfx_fdraw_glyph_frame(x, y, (facing << 8) | glyph, frame)
extern void gfx_fdraw_glyph(int x, int y, uint16_t glyph_facing);
extern void gfx_fdraw_glyph_frame(int x, int y, uint16_t glyph_facing, int frame);
# else
extern void gfx_draw_glyph(int x, int y, enum MAP_GLYPH glyph, enum MOVE_DIRECTION facing);
extern void gfx_draw_glyph_frame(int x, int y, enum MAP_GLYPH glyph, enum MOVE_DIRECTION facing, int frame);
# endif
#endif

#endif
