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

#include "diamond_girl.h"
#include "image.h"
#include "texture.h"
#include "gfx.h"
#include "gfxbuf.h"
#include "gfx_image.h"
#include "gfx_glyph.h"
#include "globals.h"
#include "gc.h"
#include "stack.h"
#include "font.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <assert.h>
#include <string.h>
#include <sys/time.h>
#ifdef WITH_OPENGL
#include <GL/glew.h>
#include <GL/glu.h>
#endif


static SDL_Surface * game_screen;
static struct image * wm_icon;

#ifdef WITH_OPENGL
static bool   gfx_initialize_opengl(Uint32 flags);
static double opengl_test_blitting(int width, int height, int blitw, int blith, int dstx, int dsty);
#endif


bool gfx_initialize(void)
{
  bool rv;

  rv = false;

  game_screen = NULL;
  wm_icon     = NULL;

  if(SDL_InitSubSystem(SDL_INIT_VIDEO) == 0)
    {
      Uint32 flags;

      wm_icon = gfx_image(GFX_IMAGE_WM_ICON);
      assert(wm_icon != NULL);
      if(wm_icon != NULL)
        SDL_WM_SetIcon(wm_icon->sdl_surface, NULL);

      flags = 0;

      if(globals.fullscreen)
        flags |= SDL_FULLSCREEN;
      else
        flags |= SDL_DOUBLEBUF;

#ifdef WITH_OPENGL
      if(globals.opengl)
        if(gfx_initialize_opengl(flags) == false)
          {
            globals.opengl = 0;
            fprintf(stderr, "OpenGL initialization failed, falling back to non-opengl mode.\n");
          }
      GFX_GL_ERROR();
#else
      globals.opengl = 0;
#endif

#ifdef WITH_NONOPENGL
      if(!globals.opengl)
        game_screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, flags);
#endif

      if(game_screen != NULL)
        {
          char title[128];

          snprintf(title, sizeof title, "Lucy the Diamond Girl v%s", VERSION);
          SDL_WM_SetCaption(title, NULL);

          struct font * font;
          char fontfn[strlen("gfx/font") + strlen("-") + (globals.language != NULL ? strlen(globals.language) : 0) + strlen("en") + 1];

          font = NULL;
          
          if(globals.language != NULL)
            {
              snprintf(fontfn, sizeof fontfn, "gfx/font-%s", globals.language);
              font = font_open(fontfn);
            }

          if(font == NULL)
            {
              snprintf(fontfn, sizeof fontfn, "gfx/font-en");
              font = font_open(fontfn);
            }
          
          assert(font != NULL);
          if(font != NULL)
            {
              GFX_GL_ERROR();
              rv = true;
            }
          else
            {
              fprintf(stderr, "Failed to load font '%s'.\n", get_data_filename(fontfn));
            }
        }
      else
        {
          fprintf(stderr, "Failed to setup screen: %s\n", SDL_GetError());
        }
    }
  else
    {
      fprintf(stderr, "Failed to initialize SDL VIDEO: %s\n", SDL_GetError());
    }

  if(rv)
    if(gfx_image_initialize() == false)
      {
        fprintf(stderr, "Failed to initialize gfx_image system.\n");
        rv = false;
      }

  if(rv)
    if(gfx_glyph_initialize() == false)
      {
        fprintf(stderr, "Failed to initialize gfx_glyph system.\n");
        rv = false;
      }

#ifdef WITH_OPENGL
  if(rv)
    if(globals.opengl)
      { /* In case we're re-opening the screen, reload all the gfxbuf's to GPU. */
        struct stack * s;
        
        s = gc_get_stack(GCT_GFXBUF);
        for(unsigned int i = 0; i < s->size; i++)
          if(s->data[i] != NULL)
            gfxbuf_load(s->data[i]);
      }
#endif

  return rv;
}

void gfx_cleanup(void)
{
  gfx_glyph_cleanup();
  gfx_image_cleanup();

#ifdef WITH_OPENGL
  if(globals.opengl)
    {
      struct stack * s;

      s = gc_get_stack(GCT_GFXBUF);
      for(unsigned int i = 0; i < s->size; i++)
        if(s->data[i] != NULL)
          gfxbuf_unload(s->data[i]);
    }
#endif

  font_close(NULL);

  SDL_QuitSubSystem(SDL_INIT_VIDEO);
}


#ifdef WITH_OPENGL

static bool gfx_initialize_opengl(Uint32 flags)
{
  bool opengl_ok;

  printf("OpenGL initialization:\n");
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#ifndef PROFILING
  if(globals.opengl_swap_control == true)
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
#endif
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

  flags |= SDL_OPENGL;
  
  opengl_ok = false;
  game_screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, flags);
  if(game_screen != NULL)
    {
      GLenum err;

      globals.opengl_max_draw_vertices = 0; /* 0 equals to unlimited */
      err = glewInit();
      if(err == GLEW_OK)
        {
          const unsigned char * tmp;

          tmp = glGetString(GL_VENDOR);
          printf(" %-30s: %s\n", "GL_VENDOR", tmp);
          tmp = glGetString(GL_RENDERER);
          printf(" %-30s: %s\n", "GL_RENDERER", tmp);
          if(!strcmp((const char *) tmp, "Mesa DRI R100 (RV200 5157)  TCL DRI2"))
            {
              globals.opengl_max_draw_vertices = 900;
              printf(" %-30s  max_draw_vertices=%d\n", "", globals.opengl_max_draw_vertices);
            }

          tmp = glGetString(GL_VERSION);
          printf(" %-30s: %s\n", "GL_VERSION", tmp);
          printf(" %-30s: %s\n", "OpenGL 1.1", GLEW_VERSION_1_1 ? "yes" : "no");
          if(GLEW_VERSION_1_1)
            opengl_ok = true;
        }
      else
        {
          /* Problem: glewInit failed, something is seriously wrong. */
          fprintf(stderr, "glewInit() failed: %s\n", glewGetErrorString(err));
        }
    }

  if(opengl_ok == true)
    {
      gfxgl_bind_texture(0);
      gfxgl_state(0, false);
      gfxgl_client_state(0, false);

      if(GLEW_VERSION_1_4)
        globals.opengl_1_4 = true;
      else
        globals.opengl_1_4 = false;
      printf(" %-30s: %s\n", "OpenGL 1.4", globals.opengl_1_4 ? "yes" : "no");
      
      if(GLEW_VERSION_1_5)
        globals.opengl_1_5 = true;
      else
        globals.opengl_1_5 = false;
      printf(" %-30s: %s\n", "OpenGL 1.5", globals.opengl_1_5 ? "yes" : "no");

#ifdef GL_VERSION_3_0
      printf(" %-30s: %s\n", "OpenGL 3.0", GLEW_VERSION_3_0 ? "yes" : "no");
#endif
      printf(" %-30s: %s\n", "ARB_vertex_buffer_object", GLEW_ARB_vertex_buffer_object ? "yes" : "no");
      GFX_GL_ERROR();

      glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
      gfxgl_state(GL_BLEND, false);
      gfxgl_state(GL_DITHER, false);
      gfxgl_state(GL_CULL_FACE, true);
      gfx_2d();
      gfxgl_client_state(GL_VERTEX_ARRAY, true);
      GFX_GL_ERROR();

      {
        GLint i;

        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &i);
	GFX_GL_ERROR();
        printf(" %-30s: %d\n", "Max texture size", (int) i);
        globals.opengl_max_texture_size = i;
      }

#ifdef GL_EXT_direct_state_access
      printf(" %-30s: ", "EXT_direct_state_access");
      if(GLEW_EXT_direct_state_access)
        printf("yes");
      else
        printf("no");
      printf("\n");
      GFX_GL_ERROR();
#endif

      printf(" %-30s: ", "ARB_texture_non_power_of_two");
      if(GLEW_ARB_texture_non_power_of_two)
        {
          if(globals.opengl_power_of_two_textures_benchmarked == false)
            { /* Test if power of two textures are faster to "blit" or not. */
              float t_pot, t_not_pot;
              int blitw, blith;
              int dstx, dsty;
                          
              printf("benchmarking...");
              fflush(stdout);
                  
              blitw = 24;
              blith = 24;

              dstx = (SCREEN_WIDTH  - blitw) / 2;
              dsty = (SCREEN_HEIGHT - blith) / 2;

              /* It seems that the first time lags a bit on some systems,
               * so the test is first run without recording the time.
               */
              opengl_test_blitting(round_to_power_of_two(blitw), round_to_power_of_two(blith), blitw, blith, dstx, dsty);

              t_pot      = opengl_test_blitting(round_to_power_of_two(blitw), round_to_power_of_two(blith), blitw, blith, dstx, dsty);
              t_not_pot  = opengl_test_blitting(blitw,                        blith,                        blitw, blith, dstx, dsty);
              t_pot     += opengl_test_blitting(round_to_power_of_two(blitw), round_to_power_of_two(blith), blitw, blith, dstx, dsty);
              t_not_pot += opengl_test_blitting(blitw,                        blith,                        blitw, blith, dstx, dsty);

              printf(" pot=%.0f, npot=%.0f => ", t_pot, t_not_pot);
              fflush(stdout);

              /* Use power of two textures if the pot is faster than the not.
               * However, favor not pot because it uses less (texture) memory. */
              if((float) t_pot * 1.1f < (float) t_not_pot)
                globals.opengl_power_of_two_textures = true;
              else
                globals.opengl_power_of_two_textures = false;
              globals.opengl_power_of_two_textures_benchmarked = true;
            }

          if(globals.opengl_power_of_two_textures == true)
            printf("disabled");
          else
            printf("yes");
        }
      else
        {
          globals.opengl_power_of_two_textures = true;
          printf("no");
        }
      printf("\n");

      printf(" %-30s: ", "ARB_texture_compression");
      if(GLEW_ARB_texture_compression)
        {
          if(globals.opengl_compressed_textures == true)
            printf("yes");
          else
            printf("disabled");
        }
      else
        {
          globals.opengl_compressed_textures = false;
          printf("no");
        }
      printf("\n");
    }

  if(opengl_ok)
    if((globals.opengl_power_of_two_textures == false && globals.opengl_max_texture_size < 240) ||
       (globals.opengl_power_of_two_textures == true  && globals.opengl_max_texture_size < 256))
      {
        printf("Error. Maximum texture size is too small. Required 256, but have %d.\n", (int) globals.opengl_max_texture_size);
        opengl_ok = false;
      }
  
  if(opengl_ok)
    {
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
      glClearDepth(1.0);
      glShadeModel(GL_SMOOTH);
      glClear(GL_DEPTH_BUFFER_BIT);
      GFX_GL_ERROR();
    }
  GFX_GL_ERROR();

  gfx_colour_clear();

  return opengl_ok;
}


static double opengl_test_blitting(int width, int height, int blitw, int blith, int dstx, int dsty)
{
  struct image * img;
  float tx1, tx2, ty1, ty2;
  struct timeval t1, t2;
  
  /* Initialize */
  gfx_2d(); /* Disables depth testing as well */
  glColor4f(1.0, 1.0, 1.0, 1.0);
  gfxgl_state(GL_TEXTURE_2D, true);
  glNormal3f(0.0f, 0.0f, 1.0f);
  
  img = image_new(width, height, false);
  image_to_texture(img, false, false, false);
  
  tx1 = 0.0f / (float) img->width;
  tx2 = tx1 + (float) blitw / (float) img->width;
  ty1 = 0.0f / (float) img->height;
  ty2 = ty1 + (float) blith / (float) img->height;

  struct gfxbuf * buffer;
  int vpos, tpos;

  buffer = gfxbuf_new(GFXBUF_STATIC_2D, GL_QUADS, GFXBUF_TEXTURE);
  gfxbuf_resize(buffer, 4);
  vpos = tpos = 0;
  
  buffer->vbuf[vpos++] = dstx;
  buffer->vbuf[vpos++] = dsty;
  buffer->tbuf[tpos++] = tx1;
  buffer->tbuf[tpos++] = 1.0f - ty1;

  buffer->vbuf[vpos++] = dstx + blitw;
  buffer->vbuf[vpos++] = dsty;
  buffer->tbuf[tpos++] = tx2;
  buffer->tbuf[tpos++] = 1.0f - ty1;

  buffer->vbuf[vpos++] = dstx + blitw;
  buffer->vbuf[vpos++] = dsty + blith;
  buffer->tbuf[tpos++] = tx2;
  buffer->tbuf[tpos++] = 1.0f - ty2;

  buffer->vbuf[vpos++] = dstx;
  buffer->vbuf[vpos++] = dsty + blith;
  buffer->tbuf[tpos++] = tx1;
  buffer->tbuf[tpos++] = 1.0f - ty2;

  gfxbuf_update(buffer, 0, vpos / 2);

  gfxgl_bind_texture(img->texture);
  
  /* Run the test */
  gettimeofday(&t1, NULL);
  for(int i = 0; i < 10000; i++)
    { /* Draw a quad with the texture, because depth testing is disabled, this should work. */
      gfxbuf_draw(buffer);
    }
      
  gfxgl_state(GL_TEXTURE_2D, false);
  gfx_flip(); /* Ensure the output is done to the display as well. */
  gettimeofday(&t2, NULL);
  
  /* Cleanup */
  glClear(GL_COLOR_BUFFER_BIT); /* Don't look so ugly for the user. */
  gfx_flip();
  image_free(img);

  /* Return */
  double rv;
  
  rv = (t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec);
  
  return rv;
}
#endif


SDL_Surface * gfx_game_screen(void)
{
  return game_screen;
}
