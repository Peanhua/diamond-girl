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

#include "diamond_girl.h"
#include "gfx.h"
#include "gfx_glyph.h"
#include "gfxbuf.h"
#include "map.h"
#include "globals.h"
#include "image.h"
#include <assert.h>
#include <SDL/SDL_image.h>
#include <errno.h>



static struct
{
  struct image * gfx;
  struct image * gfx_alt;           /* alternate graphics (ie. grayscale) */
  int            anim_frames;       /* frame count */
  int            anim_multiplier;   /* pixels to advance between frames */
  int            anim_has_facings;  /* true if the gfx has walking animation */
  int            anim_position;     /* current frame */
#ifdef WITH_OPENGL
  GLfloat * offsets[MOVE_NONE + 5];  /* Offsets to the textures, each row contains coordinates for all animation frames (anim_frames). */
#endif
} graphic_blocks[MAP_SIZEOF_];

static struct image * all_glyphs;
static struct image * all_glyphs_alt;

static int current_glyph_set;


static bool      gfx_load(enum MAP_GLYPH glyph, const char * filename);
#ifdef WITH_OPENGL
static void      gfx_glyph_calculate_texture_coordinates(enum MAP_GLYPH glyph, GLfloat x_offset, GLfloat y_offset);
#endif


bool gfx_glyph_initialize(void)
{
  bool rv;

  current_glyph_set = 0;

  for(int i = 0; i < MAP_SIZEOF_; i++)
    {
      graphic_blocks[i].gfx     = NULL;
      graphic_blocks[i].gfx_alt = NULL;
    }

  all_glyphs     = NULL;
  all_glyphs_alt = NULL;

  rv = true;
  if(rv) rv = gfx_load(MAP_EMPTY,             "empty");
  if(rv) rv = gfx_load(MAP_BORDER,            "game_border");
  if(rv) rv = gfx_load(MAP_ILLEGAL,           "illegal");
  if(rv) rv = gfx_load(MAP_SAND,              "sand");
  if(rv) rv = gfx_load(MAP_PLAYER_ARMOUR0,    "player-0");
  if(rv) rv = gfx_load(MAP_PLAYER_ARMOUR1,    "player-1");
  if(rv) rv = gfx_load(MAP_PLAYER_ARMOUR2,    "player-2");
  if(rv) rv = gfx_load(MAP_BOULDER,           "boulder");
  if(rv) rv = gfx_load(MAP_DIAMOND,           "diamond");
  if(rv) rv = gfx_load(MAP_ENEMY1,            "enemy1");
  if(rv) rv = gfx_load(MAP_ENEMY2,            "enemy2");
  if(rv) rv = gfx_load(MAP_EXIT_LEVEL,        "exit_level");
  if(rv) rv = gfx_load(MAP_BRICK,             "brick");
  if(rv) rv = gfx_load(MAP_BRICK_EXPANDING,   "brick_expanding");
  if(rv) rv = gfx_load(MAP_BRICK_UNBREAKABLE, "unbreakable_brick");
  if(rv) rv = gfx_load(MAP_BRICK_MORPHER,     "brick_morpher");
  if(rv) rv = gfx_load(MAP_SMOKE,             "smoke");
  if(rv) rv = gfx_load(MAP_EXTRA_LIFE_ANIM,   "extra_life_anim");
  if(rv) rv = gfx_load(MAP_AMEBA,             "ameba");
  if(rv) rv = gfx_load(MAP_SLIME,             "slime");

#ifdef WITH_OPENGL
  if(rv == true && globals.opengl)
    { /* Build one single texture to hold all of them. */
      int size;
      struct imagepacknode * root;
      unsigned int theorder[MAP_SIZEOF_] =
        {
          MAP_PLAYER,
          MAP_PLAYER_ARMOUR0,
          MAP_PLAYER_ARMOUR1,
          MAP_PLAYER_ARMOUR2,
          MAP_DIAMOND,
          MAP_DIAMOND_FALLING,
          MAP_ENEMY2,
          MAP_AMEBA,
          MAP_SLIME,
          MAP_ENEMY1,
          MAP_SMOKE,
          MAP_SMOKE1,
          MAP_SMOKE2,
          MAP_EXIT_LEVEL,
          MAP_EXTRA_LIFE_ANIM,
          MAP_EMPTY,
          MAP_BORDER,
          MAP_ILLEGAL,
          MAP_SAND,
          MAP_BOULDER,
          MAP_BOULDER_FALLING,
          MAP_BRICK,
          MAP_BRICK_EXPANDING,
          MAP_BRICK_UNBREAKABLE,
          MAP_BRICK_MORPHER
        };

      if(globals.opengl_power_of_two_textures == true)
        {
          if(globals.opengl_max_texture_size >= 512)
            size = 512;
          else
            size = 256;
        }
      else
        {
          if(globals.opengl_max_texture_size >= 264)
            size = 264;
          else
            size = 240;
        }

      all_glyphs     = image_new(size, size, true);
      all_glyphs_alt = image_new(size, size, true);

      root = image_pack_new(0, 0, size, size);

      for(int i = 0; i < MAP_SIZEOF_; i++)
        if(graphic_blocks[theorder[i]].gfx != NULL)
          {
            if(size < 264)
              { /* First remove some glyphs if we're low on texture space. */
                struct
                {
                  enum MAP_GLYPH glyph;
                  int            x, y;
                  int            width, height;
                } rlist[] =
                    {
                      { MAP_DIAMOND,        0, 0, 1, 1 },
                      { MAP_PLAYER_ARMOUR0, 0, 1, 3, 5 },
                      { MAP_PLAYER_ARMOUR1, 0, 1, 3, 5 },
                      { MAP_PLAYER_ARMOUR2, 0, 1, 3, 5 },
                      { MAP_SIZEOF_,        0, 0, 0, 0 }
                    };
                globals.gfx_girl_animation    = false;
                globals.gfx_diamond_animation = false;
                graphic_blocks[MAP_DIAMOND].anim_frames      = 1;
                graphic_blocks[MAP_DIAMOND].anim_multiplier  = 0;
                graphic_blocks[MAP_DIAMOND].anim_has_facings = 0;
                graphic_blocks[MAP_DIAMOND].anim_position    = 0;
                for(int j = 0; rlist[j].glyph != MAP_SIZEOF_; j++)
                  if(theorder[i] == rlist[j].glyph)
                    {
                      struct image * tmp;
                      
                      tmp = image_new(rlist[j].width * 24, rlist[j].height * 24, true);
                      image_blit_partial(graphic_blocks[theorder[i]].gfx, rlist[j].x * 24, rlist[j].y * 24, rlist[j].width * 24, rlist[j].height * 24,
                                         tmp, 0, 0);
                      image_free(graphic_blocks[theorder[i]].gfx);
                      graphic_blocks[theorder[i]].gfx = tmp;

                      tmp = image_new(rlist[j].width * 24, rlist[j].height * 24, true);
                      image_blit_partial(graphic_blocks[theorder[i]].gfx_alt, rlist[j].x * 24, rlist[j].y * 24, rlist[j].width * 24, rlist[j].height * 24,
                                         tmp, 0, 0);
                      image_free(graphic_blocks[theorder[i]].gfx_alt);
                      graphic_blocks[theorder[i]].gfx_alt = tmp;
                    }
              }

            /* Add the glyph */
            struct imagepacknode * node;

            node = image_pack(root, graphic_blocks[theorder[i]].gfx);
            if(node == NULL)
              image_save(all_glyphs, "/tmp/error.png");
            assert(node != NULL);

            image_blit(graphic_blocks[theorder[i]].gfx,     all_glyphs,     node->x, node->y);
            image_blit(graphic_blocks[theorder[i]].gfx_alt, all_glyphs_alt, node->x, node->y);

            gfx_glyph_calculate_texture_coordinates(theorder[i], node->x, node->y);

            graphic_blocks[theorder[i]].gfx     = image_free(graphic_blocks[theorder[i]].gfx    );
            graphic_blocks[theorder[i]].gfx_alt = image_free(graphic_blocks[theorder[i]].gfx_alt);
          }
      root = image_pack_free(root);

      image_to_texture(all_glyphs,     true, false, false);
      image_to_texture(all_glyphs_alt, true, false, false);

      /* Free unused copies. */
      SDL_FreeSurface(all_glyphs->sdl_surface    );
      SDL_FreeSurface(all_glyphs_alt->sdl_surface);
      all_glyphs->sdl_surface     = NULL;
      all_glyphs_alt->sdl_surface = NULL;
    }
#endif

  return rv;
}


void gfx_glyph_cleanup(void)
{
  for(int i = 0; i < MAP_SIZEOF_; i++)
    {
      if(graphic_blocks[i].gfx != NULL)
        graphic_blocks[i].gfx = image_free(graphic_blocks[i].gfx);
      if(graphic_blocks[i].gfx_alt != NULL)
        graphic_blocks[i].gfx_alt = image_free(graphic_blocks[i].gfx_alt);
#ifdef WITH_OPENGL
      for(int facing = 0; facing < MOVE_NONE + 5; facing++)
        free(graphic_blocks[i].offsets[facing]);
#endif
    }

  if(all_glyphs != NULL)
    all_glyphs = image_free(all_glyphs);
  if(all_glyphs_alt != NULL)
    all_glyphs_alt = image_free(all_glyphs_alt);
}


#ifdef WITH_NONOPENGL
# ifdef GFX_GLYPH_OPTIMIZE_PARAMETERS
void gfx_fdraw_glyph(int x, int y, uint16_t glyph_facing)
{
  assert(!globals.opengl);
  gfx_fdraw_glyph_frame(x, y, glyph_facing, graphic_blocks[glyph_facing & 0xff].anim_position);
}
# else
void gfx_draw_glyph(int x, int y, enum MAP_GLYPH glyph, enum MOVE_DIRECTION facing)
{
  assert(!globals.opengl);
  gfx_draw_glyph_frame(x, y, glyph, facing, graphic_blocks[glyph].anim_position);
}
# endif
#endif


#ifdef WITH_OPENGL
static void gfx_glyph_calculate_texture_coordinates(enum MAP_GLYPH glyph, GLfloat x_offset, GLfloat y_offset)
{
  assert(MOVE_UP    == 0);
  assert(MOVE_RIGHT == 1);
  assert(MOVE_DOWN  == 2);
  assert(MOVE_LEFT  == 3);
  assert(MOVE_NONE  == 4);
  assert(graphic_blocks[glyph].anim_frames > 0);

  for(int facing = 0; facing < MOVE_NONE + 5; facing++)
    {
      GLfloat * coords;

      coords = malloc(sizeof(GLfloat) * 4 * graphic_blocks[glyph].anim_frames);
      graphic_blocks[glyph].offsets[facing] = coords;
      assert(coords != NULL);
      if(coords != NULL)
        for(int frame = 0; frame < graphic_blocks[glyph].anim_frames; frame++)
          {
            int tx, ty;

            tx = frame * graphic_blocks[glyph].anim_multiplier;
            if(graphic_blocks[glyph].anim_has_facings)
              ty = graphic_blocks[glyph].gfx->height - (1 + facing) * 24;
            else
              ty = 0;
          
            /* x1, x2 */
            coords[frame * 4 + 0] = (x_offset + 0.5f + (GLfloat) tx        ) / (GLfloat) all_glyphs->width;
            coords[frame * 4 + 2] = (x_offset + 0.5f + (GLfloat) tx + 24.0f) / (GLfloat) all_glyphs->width;

            /* y1, y2 */
            coords[frame * 4 + 1] = (y_offset - 0.5f + (GLfloat) ty + 24.0f) / (GLfloat) all_glyphs->height;
            coords[frame * 4 + 3] = (y_offset - 0.5f + (GLfloat) ty        ) / (GLfloat) all_glyphs->height;
          }
    }
}
#endif



#ifdef WITH_OPENGL
# ifdef GFX_GLYPH_OPTIMIZE_PARAMETERS
void gfx_glyph_frender(struct gfxbuf * restrict gfxbuf, int x, int y, uint16_t glyph_facing)
{
  gfx_glyph_frender_frame(gfxbuf, x, y, glyph_facing, graphic_blocks[glyph_facing & 0xff].anim_position);
}
# else
void gfx_glyph_render(struct gfxbuf * restrict gfxbuf, int x, int y, enum MAP_GLYPH glyph, enum MOVE_DIRECTION facing)
{
  gfx_glyph_render_frame(gfxbuf, x, y, glyph, facing, graphic_blocks[glyph].anim_position);
}
# endif
#endif


#ifdef WITH_OPENGL
# ifdef GFX_GLYPH_OPTIMIZE_PARAMETERS
void gfx_glyph_frender_frame(struct gfxbuf * restrict gfxbuf, int x, int y, uint16_t glyph_facing, int frame)
{
  GLfloat * tcoords;
  int pos;

  tcoords = graphic_blocks[glyph_facing & 0xff].offsets[glyph_facing >> 8] + frame * 4;

  pos = gfxbuf->vertices * 2;

  gfxbuf->vbuf[pos + 0] = x;
  gfxbuf->vbuf[pos + 1] = y;

  gfxbuf->vbuf[pos + 2] = x + 24;
  gfxbuf->vbuf[pos + 3] = y;

  gfxbuf->vbuf[pos + 4] = x + 24;
  gfxbuf->vbuf[pos + 5] = y + 24;

  gfxbuf->vbuf[pos + 6] = x;
  gfxbuf->vbuf[pos + 7] = y + 24;


  gfxbuf->tbuf[pos + 0] = tcoords[0];
  gfxbuf->tbuf[pos + 1] = tcoords[1];

  gfxbuf->tbuf[pos + 2] = tcoords[2];
  gfxbuf->tbuf[pos + 3] = tcoords[1];

  gfxbuf->tbuf[pos + 4] = tcoords[2];
  gfxbuf->tbuf[pos + 5] = tcoords[3];

  gfxbuf->tbuf[pos + 6] = tcoords[0];
  gfxbuf->tbuf[pos + 7] = tcoords[3];


  gfxbuf->vertices += 4;
}
# else
void gfx_glyph_render_frame(struct gfxbuf * restrict gfxbuf, int x, int y, enum MAP_GLYPH glyph, enum MOVE_DIRECTION facing, int frame)
{
  GLfloat * tcoords;
  int pos;

  tcoords = graphic_blocks[glyph].offsets[facing] + frame * 4;

  pos = gfxbuf->vertices * 2;

  gfxbuf->vbuf[pos + 0] = x;
  gfxbuf->vbuf[pos + 1] = y;

  gfxbuf->vbuf[pos + 2] = x + 24;
  gfxbuf->vbuf[pos + 3] = y;

  gfxbuf->vbuf[pos + 4] = x + 24;
  gfxbuf->vbuf[pos + 5] = y + 24;

  gfxbuf->vbuf[pos + 6] = x;
  gfxbuf->vbuf[pos + 7] = y + 24;


  gfxbuf->tbuf[pos + 0] = tcoords[0];
  gfxbuf->tbuf[pos + 1] = tcoords[1];

  gfxbuf->tbuf[pos + 2] = tcoords[2];
  gfxbuf->tbuf[pos + 3] = tcoords[1];

  gfxbuf->tbuf[pos + 4] = tcoords[2];
  gfxbuf->tbuf[pos + 5] = tcoords[3];

  gfxbuf->tbuf[pos + 6] = tcoords[0];
  gfxbuf->tbuf[pos + 7] = tcoords[3];


  gfxbuf->vertices += 4;
}
# endif
#endif



void gfx_frame0(void)
{
  for(int i = 0; i < MAP_SIZEOF_; i++)
    graphic_blocks[i].anim_position = 0;
}

void gfx_next_frame(void)
{
  for(int i = 0; i < MAP_SIZEOF_; i++)
    {
      graphic_blocks[i].anim_position++;
      if(graphic_blocks[i].anim_position >= graphic_blocks[i].anim_frames)
	graphic_blocks[i].anim_position = 0;
    }
}

static bool gfx_load(enum MAP_GLYPH glyph, const char * filename)
{
  {
    char fn[1024];

    snprintf(fn, sizeof fn, "gfx/%s.png", filename);
    graphic_blocks[glyph].gfx = image_load(get_data_filename(fn), true);
    if(!globals.opengl)
      if(graphic_blocks[glyph].gfx != NULL)
        {
          SDL_Surface * tmp;
          
          tmp = SDL_ConvertSurface(graphic_blocks[glyph].gfx->sdl_surface, gfx_game_screen()->format, 0);
          if(tmp != NULL)
            {
              SDL_FreeSurface(graphic_blocks[glyph].gfx->sdl_surface);
              graphic_blocks[glyph].gfx->sdl_surface = tmp;
            }
          SDL_SetAlpha(graphic_blocks[glyph].gfx->sdl_surface, 0, SDL_ALPHA_OPAQUE);
        }

    /* Generate the alternative graphics. */
    graphic_blocks[glyph].gfx_alt = image_duplicate(graphic_blocks[glyph].gfx);
    image_to_greyscale(graphic_blocks[glyph].gfx_alt);

    if(!globals.opengl)
      {
        SDL_Surface * tmp;
          
        image_to_sdl(graphic_blocks[glyph].gfx_alt);
        tmp = SDL_ConvertSurface(graphic_blocks[glyph].gfx_alt->sdl_surface, gfx_game_screen()->format, 0);
        if(tmp != NULL)
          {
            SDL_FreeSurface(graphic_blocks[glyph].gfx_alt->sdl_surface);
            graphic_blocks[glyph].gfx_alt->sdl_surface = tmp;
          }
        SDL_SetAlpha(graphic_blocks[glyph].gfx_alt->sdl_surface, 0, SDL_ALPHA_OPAQUE);
      }
  }

     
  bool rv;

  if(graphic_blocks[glyph].gfx != NULL && graphic_blocks[glyph].gfx_alt != NULL)
    {
      FILE * fp;
      char conffn[1024];

      rv = true;

      graphic_blocks[glyph].anim_frames      = 1;
      graphic_blocks[glyph].anim_multiplier  = 24;
      graphic_blocks[glyph].anim_has_facings = 0;
      graphic_blocks[glyph].anim_position    = 0;
#ifdef WITH_OPENGL
      for(int i = 0; i < MOVE_NONE + 5; i++)
        graphic_blocks[glyph].offsets[i] = NULL;
#endif

      snprintf(conffn, sizeof conffn, "gfx/%s.png.cnf", filename);
      fp = fopen(get_data_filename(conffn), "r");
      if(fp != NULL)
	{
	  int linenr;

	  linenr = 0;
	  while(rv)
	    {
	      char linebuf[1024];

	      if(fgets(linebuf, sizeof linebuf, fp) != NULL)
		{
		  char * p;

		  p = strchr(linebuf, '\n');
		  if(p != NULL)
		    {
		      linenr++;
		      *p = '\0';
		    }

		  if(linebuf[0] != '#' && strlen(linebuf) > 0)
		    {
		      char * var, * val;

		      var = linebuf;
		      val = strchr(linebuf, '=');
		      if(val != NULL)
			{
			  long value;

			  *val = '\0';
			  val++;
			  
			  value = strtol(val, NULL, 0);
			  if(!strcmp(var, "FRAMES"))
                            graphic_blocks[glyph].anim_frames = value;
			  else if(!strcmp(var, "MULTIPLIER"))
			    graphic_blocks[glyph].anim_multiplier = value;
			  else if(!strcmp(var, "HAS_FACINGS"))
			    graphic_blocks[glyph].anim_has_facings = value;
			  else
			    fprintf(stderr, "Warning, ignoring illegal variable '%s' (value '%s') in line %d, file '%s'.\n",
				    var, val, (int) linenr, get_data_filename(conffn));
			}
		      else
			{
			  fprintf(stderr, "Warning, ignoring malformed line %d in '%s'.\n", (int) linenr, get_data_filename(conffn));
			}
		    }
		}
	      else
		{
		  if(!feof(fp))
		    {
		      int e;
		      
		      e = errno;
		      fprintf(stderr, "Failed to open gfx config file '%s' for reading: %s\n", get_data_filename(conffn), strerror(e));
		      rv = false;
		    }
		  break;
		}
	    }

	  fclose(fp);
	}
    }
  else
    {
      fprintf(stderr, "Failed to load gfx '%s': %s\n", get_data_filename(filename), SDL_GetError());
      rv = false;
    }

  return rv;
}


#ifdef WITH_NONOPENGL
# ifdef GFX_GLYPH_OPTIMIZE_PARAMETERS
void gfx_fdraw_glyph_frame(int x, int y, uint16_t glyph_facing, int frame)
{
  assert(!globals.opengl);

  uint8_t glyph;

  glyph  = glyph_facing & 0xff;
 

  SDL_Rect rdst, rsrc;

  assert(MOVE_UP    == 0);
  assert(MOVE_RIGHT == 1);
  assert(MOVE_DOWN  == 2);
  assert(MOVE_LEFT  == 3);
  assert(MOVE_NONE  == 4);

  rsrc.x = frame * graphic_blocks[glyph].anim_multiplier;
  if(graphic_blocks[glyph].anim_has_facings)
    rsrc.y = (glyph_facing >> 8) * 24;
  else
    rsrc.y = 0;
  rsrc.w = 24;
  rsrc.h = 24;
  
  rdst.x = x;
  rdst.y = y;
  rdst.w = 24;
  rdst.h = 24;
  
  SDL_Surface * src;
  
  if(current_glyph_set == 0)
    src = graphic_blocks[glyph].gfx->sdl_surface;
  else
    src = graphic_blocks[glyph].gfx_alt->sdl_surface;
  
  SDL_BlitSurface(src, &rsrc, gfx_game_screen(), &rdst);
}
# else
void gfx_draw_glyph_frame(int x, int y, enum MAP_GLYPH glyph, enum MOVE_DIRECTION facing, int frame)
{
  assert(!globals.opengl);


  SDL_Rect rdst, rsrc;

  assert(MOVE_UP    == 0);
  assert(MOVE_RIGHT == 1);
  assert(MOVE_DOWN  == 2);
  assert(MOVE_LEFT  == 3);
  assert(MOVE_NONE  == 4);

  rsrc.x = frame * graphic_blocks[glyph].anim_multiplier;
  if(graphic_blocks[glyph].anim_has_facings)
    rsrc.y = facing * 24;
  else
    rsrc.y = 0;
  rsrc.w = 24;
  rsrc.h = 24;
  
  rdst.x = x;
  rdst.y = y;
  rdst.w = 24;
  rdst.h = 24;
  
  SDL_Surface * src;
  
  if(current_glyph_set == 0)
    src = graphic_blocks[glyph].gfx->sdl_surface;
  else
    src = graphic_blocks[glyph].gfx_alt->sdl_surface;
  
  SDL_BlitSurface(src, &rsrc, gfx_game_screen(), &rdst);
}
# endif
#endif

void gfx_set_current_glyph_set(int glyph_set)
{
#ifdef WITH_OPENGL
  if(globals.opengl)
    {
      assert(glyph_set == 0 || glyph_set == 1);
      if(glyph_set == 0)
        gfx_bind_texture(all_glyphs->texture);
      else if(glyph_set == 1)
        gfx_bind_texture(all_glyphs_alt->texture);
    }
#endif

  current_glyph_set = glyph_set;
}




uint8_t gfx_get_frame(enum MAP_GLYPH glyph)
{
  assert(glyph < MAP_SIZEOF_);

  return graphic_blocks[glyph].anim_position;
}

uint8_t gfx_get_maxframe(enum MAP_GLYPH glyph)
{
  assert(glyph < MAP_SIZEOF_);

  return graphic_blocks[glyph].anim_frames;
}
