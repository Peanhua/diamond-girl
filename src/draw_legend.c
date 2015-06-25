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
#include "font.h"
#include "gfx.h"
#include "gfxbuf.h"
#include "gfx_glyph.h"
#include "globals.h"
#include "map.h"
#include <assert.h>
#include <libintl.h>

#ifdef WITH_OPENGL
static struct gfxbuf * buffer = NULL;
#endif

void draw_legend(int x, int y)
{
  struct
  {
    enum MAP_GLYPH      glyph;
    enum MOVE_DIRECTION mdir;
    const char *        explanation;
  } explanations[] = 
      {
        { MAP_PLAYER_ARMOUR1,    MOVE_NONE, gettext("You, or Lucy the girl.") },
        { MAP_DIAMOND,           MOVE_UP,   gettext("A diamond, collect.") },
        { MAP_BOULDER,           MOVE_UP,   gettext("A boulder.") },
        { MAP_SAND,              MOVE_UP,   gettext("Sand.") },
        { MAP_EXTRA_LIFE_ANIM,   MOVE_UP,   gettext("For few seconds, empty space looks like") },
        { MAP_EXTRA_LIFE_ANIM,   MOVE_UP,   gettext("  this when you get extra girl.") },
        { MAP_BRICK,             MOVE_UP,   gettext("A brick wall.") },
        { MAP_BRICK_MORPHER,     MOVE_UP,   gettext("A morphing wall, turns diamonds into") },
        { MAP_BRICK_MORPHER,     MOVE_UP,   gettext("  boulders and boulders into diamonds.") },
        { MAP_BRICK_UNBREAKABLE, MOVE_UP,   gettext("Unbreakable brick wall.") },
        { MAP_AMEBA,             MOVE_UP,   gettext("Ameba.") },
        { MAP_SLIME,             MOVE_UP,   gettext("Slime.") },
        { MAP_EXIT_LEVEL,        MOVE_UP,   gettext("Exit to the next level.") },
        { MAP_ENEMY1,            MOVE_UP,   gettext("A radioactive bug, avoid.") },
        { MAP_ENEMY2,            MOVE_UP,   gettext("A radioactive butterfly, avoid.") },
        { MAP_SIZEOF_,           MOVE_UP,   NULL }
      };
  int yadj;

  if(font_height() > 26)
    yadj = font_height();
  else
    yadj = 26;


#ifdef WITH_OPENGL
  if(globals.opengl)
    if(buffer == NULL)
      {
        buffer = gfxbuf_new(GFXBUF_STATIC_2D, GL_QUADS, GFXBUF_TEXTURE | GFXBUF_BLENDING);
        assert(buffer != NULL);
        if(buffer != NULL)
          {
            int i;
            
            gfxbuf_resize(buffer, 4 * MAP_SIZEOF_);
            
            buffer->vertices = 0;
            i = 0;
            while(explanations[i].glyph != MAP_SIZEOF_)
              {
                gfx_glyph_render_frame(buffer, 0, i * yadj, explanations[i].glyph, explanations[i].mdir, 0);
                i++;
              }
            
            gfxbuf_update(buffer, 0, buffer->vertices);
          }
      }
#endif

  int i;

  i = 0;
  while(explanations[i].glyph != MAP_SIZEOF_)
    {
      int tx, ty;

      tx = x + 30;
      ty = (y + i * yadj) + (yadj - font_height()) / 2;

      font_write(tx, ty, explanations[i].explanation);

#ifdef WITH_NONOPENGL
      if(!globals.opengl)
        gfx_draw_glyph_frame(x, y + i * yadj, explanations[i].glyph, explanations[i].mdir, 0);
#endif

      i++;
    }

#ifdef WITH_OPENGL	    
  if(globals.opengl)
    {
      gfx_set_current_glyph_set(0);
      gfx_colour_white();

      gfxbuf_draw_init(buffer);
      gfxbuf_draw_at(buffer, x, y);
    }
#endif

}
