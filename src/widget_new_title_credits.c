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
#include "widget.h"
#include "gfx.h"
#include "gfxbuf.h"
#include "globals.h"

#include <unistd.h>
#include <assert.h>
#include <errno.h>

static char * filenames[] =
  {
    "data/Welcome",
    NULL
  };



static void   draw(struct widget * this);
static char * read_credits(char const * const filename);
static void   on_unload(struct widget * this);
static char * credits_padding(char * credits, bool long_delay);

struct widget * widget_new_title_credits(struct widget * parent, int x, int y, int width)
{
  struct widget * obj;

  obj = widget_new_child(parent, x, y, width, 10); /* The height is in error here, but it should not matter. */
  assert(obj != NULL);
  if(obj != NULL)
    {
      widget_set_on_draw(obj, draw);
      widget_set_ulong(obj, "type", WT_TITLE_CREDITS);
      widget_set_flags(obj, 0);
      widget_set_ulong(obj, "help_screen", 0);
      widget_set_ulong(obj, "credits_index", 0);
      widget_set_pointer(obj, "credits", 'P', read_credits(filenames[0]));
      widget_set_on_unload(obj, on_unload);

#ifdef WITH_OPENGL
      if(globals.opengl)
        {
          struct gfxbuf * buf;

          buf = gfxbuf_new(GFXBUF_STATIC_2D, GL_QUADS, GFXBUF_COLOUR | GFXBUF_BLENDING);
          assert(buf != NULL);
          gfxbuf_resize(buf, 4 * 2);

          int vpos, cpos;

          vpos = cpos = 0;

          buf->vbuf[vpos++] = 0;
          buf->vbuf[vpos++] = font_height();
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0xff;

          buf->vbuf[vpos++] = 0;
          buf->vbuf[vpos++] = 0;
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0xff;

          buf->vbuf[vpos++] = 12;
          buf->vbuf[vpos++] = 0;
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0x00;

          buf->vbuf[vpos++] = 12;
          buf->vbuf[vpos++] = font_height();
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0x00;


          buf->vbuf[vpos++] = width - 12;
          buf->vbuf[vpos++] = font_height();
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0x00;

          buf->vbuf[vpos++] = width - 12;
          buf->vbuf[vpos++] = 0;
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0x00;

          buf->vbuf[vpos++] = width;
          buf->vbuf[vpos++] = 0;
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0xff;

          buf->vbuf[vpos++] = width;
          buf->vbuf[vpos++] = font_height();
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0x00;
          buf->cbuf[cpos++] = 0xff;

          gfxbuf_update(buf, 0, vpos / 2);

          widget_set_pointer(obj, "gfxbuf", 'P', buf);
        }
#endif
    }

  return obj;
}


/* Change the currently scrolling credits to something else.
 * When the "something else" has been scrolled, resume the normal operation.
 */
void widget_title_credits_set(struct widget * widget, char * text, bool long_delay)
{
  if(widget_get_pointer(widget, "saved_credits", 'P') == NULL)
    { /* Save the current credits. */
      widget_set_pointer(widget, "saved_credits", 'P',   widget_get_pointer(widget, "credits", 'P'));
      widget_set_ulong(widget,   "saved_credits_pos",    widget_get_ulong(widget,   "credits_pos"));
      widget_set_ulong(widget,   "saved_credits_cc",     widget_get_ulong(widget,   "credits_cc"));
      widget_set_ulong(widget,   "saved_credits_offset", widget_get_ulong(widget,   "credits_offset"));
    }

  char * credits;

  credits = credits_padding(text, long_delay);

  widget_set_ulong(widget,   "credits_cc",     0);
  widget_set_ulong(widget,   "credits_offset", 0);
  widget_set_ulong(widget,   "credits_pos",    0);
  widget_set_pointer(widget, "credits", 'P',   credits);
}


static void on_unload(struct widget * this)
{
  char * credits;

  credits = widget_get_pointer(this, "credits", 'P');
  if(credits != NULL)
    free(credits);

#ifdef WITH_OPENGL
  if(globals.opengl)
    {
      struct gfxbuf * buf;

      buf = widget_get_pointer(this, "gfxbuf", 'P');
      if(buf != NULL)
        buf = gfxbuf_free(buf);
    }
#endif
}

static char * read_credits(char const * const filename)
{ /* read credits */
  char * credits;
  char * tmp;
  int    credits_length;
  char * default_credits = "Lucy the Diamond Girl (C) 2005-2015 Joni Yrjana";
  char fn[strlen(filename) + strlen("-en.txt") + 1];
  bool readok;
  
  credits = NULL;

  readok  = false;
  if(globals.language != NULL)
    {
      snprintf(fn, sizeof fn, "%s-%s.txt", filename, globals.language);
      readok = read_file(get_data_filename(fn), &tmp, &credits_length);
    }
  if(readok == false)
    {
      snprintf(fn, sizeof fn, "%s-en.txt", filename);
      readok = read_file(get_data_filename(fn), &tmp, &credits_length);
    }
  
  if(readok == true)
    {
      assert(tmp != NULL);
      if(tmp != NULL)
        {
          assert((int) strlen(tmp) == credits_length);
          credits = credits_padding(tmp, true);
        }
    }
  else
    {
      fprintf(stderr, "Failed to read '%s'.\n", filename);
    }

  if(credits == NULL)
    credits = credits_padding(default_credits, true);

  assert(credits != NULL);

  return credits;
}


static void draw(struct widget * this)
{
  int    left_x;
  char * credits;
  int    credits_pos;
  bool   skip_to_next;

  left_x      = widget_x(this);
  credits     = widget_get_pointer(this, "credits", 'P');
  credits_pos = widget_get_ulong(this, "credits_pos");

  skip_to_next = false;

  int x, y;

  y = widget_y(this);

  /* Determine the start character (current_character) and offset (width) to write text. */
  int current_character, maxlen;
  int width, tmpwidth;

  current_character = widget_get_ulong(this, "credits_cc");
  width = widget_get_ulong(this, "credits_offset");
  maxlen = strlen(credits);
  do
    {
      tmpwidth = font_nwidth(credits, current_character + 1);
      if(tmpwidth < credits_pos)
        {
          current_character++;
          width = tmpwidth;
        }
    }
  while(tmpwidth < credits_pos && current_character < maxlen);

  if(current_character >= maxlen)
    {
      credits_pos = 0;
      current_character = 0;
      width = 0;
      skip_to_next = true;
    }

  if(skip_to_next == false)
    {
      x = left_x - credits_pos + width; /* This will be close to left_x. */
      
      char buf[128]; /* Limit the number of characters to be written to 128, which seems to be enough,
                      * with 80 the start was jerky because empty space takes so little space:
                      *   the rightmost character was not clipped nor under the black "fadeblock".
                      */
      strncpy(buf, credits + current_character, sizeof buf);
      buf[sizeof buf - 1] = '\0';

      if(globals.opengl)
        {
#ifdef WITH_OPENGL
          struct gfxbuf * buffer;

          buffer = widget_get_pointer(this, "gfxbuf", 'P');
          assert(buffer != NULL);

          glScissor(left_x, SCREEN_HEIGHT - y - font_height(), widget_width(this), font_height());
          gfxgl_state(GL_SCISSOR_TEST, true);

          font_write(x, y, buf);

          gfxbuf_draw_init(buffer);
          gfxbuf_draw_at(buffer, left_x, y);

          gfxgl_state(GL_SCISSOR_TEST, false);
#endif
        }
      else
        {
#ifdef WITH_NONOPENGL
          SDL_Rect r;
      
          r.x = left_x;
          r.y = y;
          r.w = widget_width(this);
          r.h = font_height();
          SDL_SetClipRect(gfx_game_screen(), &r);

          font_write(x, y, buf);

          SDL_SetClipRect(gfx_game_screen(), NULL);
#endif
        }

      credits_pos++;
    }


  widget_set_ulong(this, "credits_cc",     current_character);
  widget_set_ulong(this, "credits_offset", width            );
  widget_set_ulong(this, "credits_pos",    credits_pos      );


  if(skip_to_next == true)
    {
      free(credits);

      if(widget_get_pointer(this, "saved_credits", 'P') != NULL)
        { /* Restore saved credits. */
          widget_set_pointer(this, "credits", 'P',   widget_get_pointer(this, "saved_credits", 'P'));
          widget_set_ulong(this,   "credits_pos",    widget_get_ulong(this,   "saved_credits_pos"));
          widget_set_ulong(this,   "credits_cc",     widget_get_ulong(this,   "saved_credits_cc"));
          widget_set_ulong(this,   "credits_offset", widget_get_ulong(this,   "saved_credits_offset"));

          widget_set_pointer(this, "saved_credits", 'P', NULL);
        }
      else
        { /* Advance to the next credits file. */
          int ind;

          ind = widget_get_ulong(this, "credits_index");
          ind++;

          if(filenames[ind] == NULL)
            ind = 0;
          credits = read_credits(filenames[ind]);

          widget_set_ulong(this, "credits_index", ind);
          widget_set_pointer(this, "credits", 'P', credits);
        }
    }
}


/* Pad the credits with spaces in front so that they appear from the right. */
static char * credits_padding(char * credits, bool long_delay)
{
  int lf;
  int credits_length;

  credits_length = strlen(credits);

  lf = 0;
  for(int i = 0; i < credits_length; i++)
    if(credits[i] == '\n')
      lf++;

  int count;
  const char * lf_text = "      ";
  char * newcredits;

  if(long_delay)
    count = 32;
  else
    count = 15;
  newcredits = malloc(count * credits_length + // The beginning empty space.
                      credits_length         + // The actual contents.
                      lf * credits_length    + // Each linefeed is replaced with the string.
                      1);                      // EOS.
  assert(newcredits != NULL);
  if(newcredits != NULL)
    {
      char * dst;

      dst = newcredits;

      /* The beginning empty space. */
      for(int i = 0; i < count; i++)
        {
          memcpy(dst, lf_text, strlen(lf_text));
          dst += strlen(lf_text);
        }

      /* The actual contents with linefeed replacement. */
      for(int i = 0; i < credits_length; i++)
        {
          if(credits[i] == '\n')
            {
              memcpy(dst, lf_text, strlen(lf_text));
              dst += strlen(lf_text);
            }
          else
            {
              *dst = credits[i];
              dst++;
            }
        }
          
      /* EOS. */
      *dst = '\0';

      credits = newcredits;
    }
  else
    {
      fprintf(stderr, "Failed to allocate memory: %s\n", strerror(errno)); 
      credits = NULL;
    }

  return credits;
}
