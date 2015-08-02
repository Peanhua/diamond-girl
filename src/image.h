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

#ifndef IMAGE_H
#define IMAGE_H

#include <inttypes.h>
#include <stdbool.h>
#include <SDL/SDL.h>
#ifdef WITH_OPENGL
#include <GL/glew.h>
#endif

struct gfxbuf;

#define IF_ALPHA      (1<<0)
#define IF_MIPMAPPING (1<<1)
#define IF_LINEAR     (1<<2)

struct image
{
  uint8_t *     data;
  int           width;
  int           height;
  int           content_width;
  int           content_height;
  SDL_Surface * sdl_surface;
  uint8_t       flags;
#ifdef WITH_OPENGL
  GLuint          texture;
  bool            texture_initialized;
  bool            texture_wanted;
  struct image *  texture_atlas;
  float           texture_offset_x;
  float           texture_offset_y;
  struct gfxbuf * buffer;
  int             bufferw, bufferh;
#endif
#ifndef NDEBUG
  char * backtrace;
#endif
};

struct imagepacknode
{
  struct image *         image;
  struct imagepacknode * left;
  struct imagepacknode * right;
  uint16_t x;
  uint16_t y;
  uint16_t w;
  uint16_t h;
};

extern struct image * image_new(int width, int height, bool alpha);
extern struct image * image_free(struct image * image);
extern struct image * image_load(const char * filename, bool generate_alpha);
extern void           image_save(struct image const * image, const char * filename);
extern struct image * image_load_from_surface(SDL_Surface * source, bool generate_alpha);
extern struct image * image_duplicate(struct image const * image);
extern bool           image_expand(struct image * image, int new_width, int new_height);
extern bool           image_to_sdl(struct image * image);
extern void           image_to_greyscale(struct image * image);
extern void           image_blit(struct image const * src, struct image * dst, int dst_x, int dst_y);
extern void           image_blit_partial(struct image const * src, int src_x, int src_y, int width, int height, struct image * dst, int dst_x, int dst_y);
extern void           image_blit_rotated(struct image const * src, int degrees, struct image * dst, int dst_x, int dst_y);
extern struct image * image_trim(struct image const * image); // Returns a new image.

#ifndef NDEBUG
extern void           image_dump(struct image const * image, unsigned int indentation);
#endif

#ifdef WITH_OPENGL
extern void           image_to_texture(struct image * image, bool use_mipmapping, bool linear);
extern void           image_free_texture(struct image * image);
extern bool           image_setup_buffer(struct image * image, bool alpha);

extern struct imagepacknode * image_pack_new(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
extern struct imagepacknode * image_pack_free(struct imagepacknode * node);
extern struct imagepacknode * image_pack(struct imagepacknode * node, struct image * image);
extern void                   image_pack_dump(struct imagepacknode * node);
#endif

#endif
