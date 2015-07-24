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
#include "gfx_image.h"
#include "image.h"
#include "stack.h"
#include "td_object.h"
#include "texture.h"
#include "treasure.h"
#include "globals.h"
#include <assert.h>
#include <errno.h>
#include <SDL_rotozoom.h>

#ifdef WITH_OPENGL
#include "treasure_colours.c"
#endif


static struct
{
  enum GFX_IMAGE const image_id;
  struct image *       image;
  char const * const   filename;
  bool const           alpha;
  bool const           texturize;
  bool const           mipmapping;
  bool const           greyscale;
  bool const           localized;
  unsigned int const   texture_atlas_group; // 0 = no atlas, 1 = first atlas, .. up to TEXTUREATLAS_COUNT defined below
} images[GFX_IMAGE_SIZEOF_] =
  {
    { GFX_IMAGE_CHECKBOX_ON,                   NULL, "gfx/checkbox_on",           true,  true, false, false, false, 2 },
    { GFX_IMAGE_INTERMISSION,                  NULL, "gfx/intermission",          true,  true,  true, false, true,  0 },
    { GFX_IMAGE_MAP_CURSOR,                    NULL, "gfx/map_cursor",            true,  true,  true, false, false, 2 },
    { GFX_IMAGE_WIDGET_SELECT,                 NULL, "gfx/widget_select",         true,  true, false, false, false, 2 },
    { GFX_IMAGE_LOGO,                          NULL, "gfx/logo",                 false, false, false, false, false, 0 },
    { GFX_IMAGE_CLOSE_BUTTON,                  NULL, "gfx/close_button",          true,  true, false, false, false, 2 },
    { GFX_IMAGE_SCORE_BACKGROUND,              NULL, "gfx/game_score",           false,  true, false, false, false, 0 },
    { GFX_IMAGE_SCORE_BACKGROUND_256,          NULL, "gfx/game_score",           false,  true, false, false, false, 0 },
    { GFX_IMAGE_WM_ICON,                       NULL, "gfx/desktop-icon",         false, false, false, false, false, 0 },
    { GFX_IMAGE_PLAY_BUTTON,                   NULL, "gfx/play",                  true,  true, false, false, false, 2 },
    { GFX_IMAGE_STOP_BUTTON,                   NULL, "gfx/stop",                  true,  true, false, false, false, 2 },
    { GFX_IMAGE_REWIND_BUTTON,                 NULL, "gfx/rewind",                true,  true, false, false, false, 2 },
    { GFX_IMAGE_FORWARD_BUTTON,                NULL, "gfx/forward",               true,  true, false, false, false, 2 },
    { GFX_IMAGE_EJECT_BUTTON,                  NULL, "gfx/eject",                 true,  true, false, false, false, 2 },
    { GFX_IMAGE_MODE_SWITCH_LEFT,              NULL, "gfx/mode_switch_left",      true,  true, false, false, false, 2 },
    { GFX_IMAGE_MODE_SWITCH_UP,                NULL, "gfx/mode_switch_up",        true,  true, false, false, false, 2 },
    { GFX_IMAGE_MODE_SWITCH_RIGHT,             NULL, "gfx/mode_switch_right",     true,  true, false, false, false, 2 },
    { GFX_IMAGE_BOOKPILE,                      NULL, "gfx/bookpile",              true,  true, false, false, false, 0 },
    { GFX_IMAGE_CAFE,                          NULL, "gfx/cafe",                  true,  true, false, false, false, 0 },
    { GFX_IMAGE_DIARY,                         NULL, "gfx/diary",                 true,  true, false, false, false, 0 },
    { GFX_IMAGE_GRAVESTONE,                    NULL, "gfx/gravestone",            true,  true, false, false, false, 0 },
    { GFX_IMAGE_NOTES,                         NULL, "gfx/notes",                 true,  true, false, false, false, 0 },
    { GFX_IMAGE_TREASURESTAND,                 NULL, "gfx/treasurestand",         true,  true, false, false, false, 0 },
    { GFX_IMAGE_TRAIT_ADVENTURE_MODE,          NULL, "gfx/trait_adventure_mode",  true,  true,  true, false, false, 1 },
    { GFX_IMAGE_TRAIT_ADVENTURE_MODE_DISABLED, NULL, "gfx/trait_adventure_mode",  true,  true,  true,  true, false, 1 },
    { GFX_IMAGE_TRAIT_RIBBON,                  NULL, "gfx/trait_ribbon",          true,  true,  true, false, false, 1 },
    { GFX_IMAGE_TRAIT_RIBBON_DISABLED,         NULL, "gfx/trait_ribbon",          true,  true,  true,  true, false, 1 },
    { GFX_IMAGE_TRAIT_GREEDY,                  NULL, "gfx/trait_greedy",          true,  true,  true, false, false, 1 },
    { GFX_IMAGE_TRAIT_GREEDY_DISABLED,         NULL, "gfx/trait_greedy",          true,  true,  true,  true, false, 1 },
    { GFX_IMAGE_TRAIT_TIME_CONTROL,            NULL, "gfx/trait_time_control",    true,  true,  true, false, false, 1 },
    { GFX_IMAGE_TRAIT_TIME_CONTROL_DISABLED,   NULL, "gfx/trait_time_control",    true,  true,  true,  true, false, 1 },
    { GFX_IMAGE_TRAIT_POWER_PUSH,              NULL, "gfx/trait_power_push",      true,  true,  true, false, false, 1 },
    { GFX_IMAGE_TRAIT_POWER_PUSH_DISABLED,     NULL, "gfx/trait_power_push",      true,  true,  true,  true, false, 1 },
    { GFX_IMAGE_TRAIT_DIAMOND_PUSH,            NULL, "gfx/trait_diamond_push",    true,  true,  true, false, false, 1 },
    { GFX_IMAGE_TRAIT_DIAMOND_PUSH_DISABLED,   NULL, "gfx/trait_diamond_push",    true,  true,  true,  true, false, 1 },
    { GFX_IMAGE_TRAIT_RECYCLER,                NULL, "gfx/trait_recycler",        true,  true,  true, false, false, 1 },
    { GFX_IMAGE_TRAIT_RECYCLER_DISABLED,       NULL, "gfx/trait_recycler",        true,  true,  true,  true, false, 1 },
    { GFX_IMAGE_TRAIT_STARS1,                  NULL, "gfx/trait_stars1",          true,  true,  true, false, false, 1 },
    { GFX_IMAGE_TRAIT_STARS1_DISABLED,         NULL, "gfx/trait_stars1",          true,  true,  true,  true, false, 1 },
    { GFX_IMAGE_TRAIT_STARS2,                  NULL, "gfx/trait_stars2",          true,  true,  true, false, false, 1 },
    { GFX_IMAGE_TRAIT_STARS2_DISABLED,         NULL, "gfx/trait_stars2",          true,  true,  true,  true, false, 1 },
    { GFX_IMAGE_TRAIT_STARS3,                  NULL, "gfx/trait_stars3",          true,  true,  true, false, false, 1 },
    { GFX_IMAGE_TRAIT_STARS3_DISABLED,         NULL, "gfx/trait_stars3",          true,  true,  true,  true, false, 1 },
    { GFX_IMAGE_TRAIT_CHAOS,                   NULL, "gfx/trait_chaos",           true,  true,  true, false, false, 1 },
    { GFX_IMAGE_TRAIT_CHAOS_DISABLED,          NULL, "gfx/trait_chaos",           true,  true,  true,  true, false, 1 },
    { GFX_IMAGE_TRAIT_DYNAMITE,                NULL, "gfx/trait_dynamite",        true,  true,  true, false, false, 1 },
    { GFX_IMAGE_TRAIT_DYNAMITE_DISABLED,       NULL, "gfx/trait_dynamite",        true,  true,  true,  true, false, 1 },
    { GFX_IMAGE_TRAIT_KEY,                     NULL, "gfx/trait_key",             true,  true,  true, false, false, 1 },
    { GFX_IMAGE_TRAIT_KEY_DISABLED,            NULL, "gfx/trait_key",             true,  true,  true,  true, false, 1 },
    { GFX_IMAGE_TRAIT_IRON_GIRL,               NULL, "gfx/trait_iron_girl",       true,  true,  true, false, false, 1 },
    { GFX_IMAGE_TRAIT_IRON_GIRL_DISABLED,      NULL, "gfx/trait_iron_girl",       true,  true,  true,  true, false, 1 },
    { GFX_IMAGE_TRAIT_PYJAMA_PARTY,            NULL, "gfx/trait_pyjama_party",    true,  true,  true, false, false, 1 },
    { GFX_IMAGE_TRAIT_PYJAMA_PARTY_DISABLED,   NULL, "gfx/trait_pyjama_party",    true,  true,  true,  true, false, 1 },
    { GFX_IMAGE_TRAIT_QUESTS,                  NULL, "gfx/trait_quests",          true,  true,  true, false, false, 1 },
    { GFX_IMAGE_TRAIT_QUESTS_DISABLED,         NULL, "gfx/trait_quests",          true,  true,  true,  true, false, 1 },
    { GFX_IMAGE_TRAIT_EDIT,                    NULL, "gfx/trait_edit",            true,  true,  true, false, false, 1 },
    { GFX_IMAGE_TRAIT_EDIT_DISABLED,           NULL, "gfx/trait_edit",            true,  true,  true,  true, false, 1 },
    { GFX_IMAGE_TRAIT_QUICK_CONTACT,           NULL, "gfx/trait_quick_contact",   true,  true,  true, false, false, 1 },
    { GFX_IMAGE_TRAIT_QUICK_CONTACT_DISABLED,  NULL, "gfx/trait_quick_contact",   true,  true,  true,  true, false, 1 }
  };

#ifdef WITH_OPENGL
#define TEXTUREATLAS_COUNT 2
struct image *         textureatlas_images[TEXTUREATLAS_COUNT];
struct imagepacknode * textureatlas_roots[TEXTUREATLAS_COUNT];
#endif



struct treasure_image
{
  bool            greyscale;
  struct treasure reference;
  struct image *  image;
};

struct stack * treasure_images = NULL;


#if WITH_OPENGL
struct treasure_td_object
{
  struct treasure    reference;
  struct td_object * td_object;
};

struct stack * treasure_td_objects = NULL;
#endif


bool gfx_image_initialize(void)
{
  bool rv;

  rv = true;
  for(int i = 0; i < GFX_IMAGE_SIZEOF_; i++)
    {
      images[i].image = NULL;
      assert(images[i].image_id == (enum GFX_IMAGE) i);
    }

  treasure_images = stack_new();
  if(treasure_images == NULL)
    rv = false;

#ifdef WITH_OPENGL
  treasure_td_objects = stack_new();
  if(treasure_td_objects == NULL)
    rv = false;


  /* Generate texture atlases. */
  for(int i = 0; rv == true && i < TEXTUREATLAS_COUNT; i++)
    if(globals.opengl == true && globals.opengl_max_texture_size >= 256)
      {
        textureatlas_images[i] = image_new(256, 256, true);
        textureatlas_roots[i]  = image_pack_new(0, 0, 256, 256);
        if(textureatlas_images[i] == NULL || textureatlas_roots[i] == NULL)
          rv = false;
      }
    else
      {
        textureatlas_images[i] = NULL;
        textureatlas_roots[i] = NULL;
      }
  /* Load images belonging to a texture atlas group, and setup their gfxbuf buffers. */
  for(int i = 0; rv == true && i < GFX_IMAGE_SIZEOF_; i++)
    if(images[i].texture_atlas_group > 0)
      {
        struct image * image;
        
        image = gfx_image(i);
        if(image != NULL)
          image_setup_buffer(image, true);
        else
          rv = false;
      }
  /* Generate atlas textures. */
  for(int i = 0; rv == true && i < TEXTUREATLAS_COUNT; i++)
    if(textureatlas_images[i] != NULL)
      {
        image_to_texture(textureatlas_images[i], true, false, true);
#if 0
        char fn[1024];
        snprintf(fn, sizeof fn, "/tmp/atlas-%d.png", (int) i);
        image_save(textureatlas_images[i], fn);
#endif
      }
#endif
  
  return rv;
}


void gfx_image_cleanup(void)
{
  for(int i = 0; i < GFX_IMAGE_SIZEOF_; i++)
    if(images[i].image != NULL)
      images[i].image = image_free(images[i].image);

  if(treasure_images != NULL)
    for(unsigned int i = 0; i < treasure_images->size; i++)
      {
        struct treasure_image * ti;

        ti = treasure_images->data[i];
        ti->image = image_free(ti->image);
        free(ti);
      }
  treasure_images = stack_free(treasure_images);

#ifdef WITH_OPENGL
  if(treasure_td_objects != NULL)
    for(unsigned int i = 0; i < treasure_td_objects->size; i++)
      {
        struct treasure_td_object * tto;

        tto = treasure_td_objects->data[i];
        tto->td_object = td_object_unload(tto->td_object);
        free(tto);
      }
  treasure_td_objects = stack_free(treasure_td_objects);

  for(int i = 0; i < TEXTUREATLAS_COUNT; i++)
    {
      if(textureatlas_roots[i] != NULL)
        textureatlas_roots[i] = image_pack_free(textureatlas_roots[i]);

      if(textureatlas_images[i] != NULL)
        textureatlas_images[i] = image_free(textureatlas_images[i]);
    }
#endif
}


struct image * gfx_image(enum GFX_IMAGE image_id)
{
  struct image * image;

  image = images[image_id].image;
  if(image == NULL)
    {
      char fn[1024];
      
      if(images[image_id].localized == true)
        {
          snprintf(fn, sizeof fn, "%s-%s.png", images[image_id].filename, globals.language);
          image = image_load(get_data_filename(fn), images[image_id].alpha);
        }

      if(image == NULL)
        {
          snprintf(fn, sizeof fn, "%s.png", images[image_id].filename);
          image = image_load(get_data_filename(fn), images[image_id].alpha);
        }

      if(image_id == GFX_IMAGE_SCORE_BACKGROUND_256)
        {
          SDL_Surface * tmp;

          assert(image->height <= 256);
          tmp = zoomSurface(image->sdl_surface, (float) (256 + 1) / (float) image->width, 1.0f, 1);
          assert(tmp != NULL);
          if(tmp != NULL)
            {
              struct image * ni;

              ni = image_load_from_surface(tmp, images[image_id].alpha);
              assert(ni != NULL);
              if(ni != NULL)
                {
                  image_free(image);
                  image = ni;
                }
            }
        }

      images[image_id].image = image;

      if(image != NULL)
        {
          if(images[image_id].greyscale == true)
            image_to_greyscale(image);

#ifdef WITH_OPENGL
          if(globals.opengl)
            {
              if(globals.opengl_power_of_two_textures == true)
                {
                  int origw, origh, w, h;
                  
                  origw = image->width;
                  origh = image->height;
                  w = round_to_power_of_two(origw);
                  h = round_to_power_of_two(origh);
                  
                  if(w != origw || h != origh)
                    image_expand(image, w, h);
                }
              
              if(images[image_id].texture_atlas_group > 0 &&
                 textureatlas_roots[images[image_id].texture_atlas_group - 1] != NULL &&
                 textureatlas_images[images[image_id].texture_atlas_group - 1] != NULL   )
                {
                  struct imagepacknode * root;
                  struct imagepacknode * node;
                  struct image *         atlas;

                  assert(images[image_id].alpha == true);
                  root  = textureatlas_roots[images[image_id].texture_atlas_group - 1];
                  atlas = textureatlas_images[images[image_id].texture_atlas_group - 1];
                  node = image_pack(root, image);
                  if(node == NULL)
                    image_save(atlas, "/tmp/error.png");
                  assert(node != NULL);

                  /* Copy image pixels to the atlas. */
                  image_blit(image, atlas, node->x, node->y);
                  /* Update image data. The texture is set in gfx_image_initialize(). */
                  image->texture_atlas       = atlas;
                  image->texture_initialized = true;
                  image->texture_offset_x    = (double) node->x / (double) atlas->width;
                  image->texture_offset_y    = (double) node->y / (double) atlas->height;
                }
              else if(images[image_id].texturize == true)
                image_to_texture(image, images[image_id].alpha, images[image_id].mipmapping, true);
            }
#endif
        }
    }

  assert(image != NULL);

  return image;
}



struct image * gfx_image_treasure(struct treasure * treasure, bool greyscale)
{
  struct image * image;

  image = NULL;
  assert(treasure_images != NULL);
  for(unsigned int i = 0; image == NULL && i < treasure_images->size; i++)
    {
      struct treasure_image * ti;

      ti = treasure_images->data[i];
      if(ti->greyscale           == greyscale           &&
         ti->reference.type      == treasure->type      &&
         ti->reference.material  == treasure->material  &&
         ti->reference.gemstones == treasure->gemstones    )
        image = ti->image;
    }

  if(image == NULL)
    {
      struct treasure_image * ti;

      ti = malloc(sizeof *ti);
      assert(ti != NULL);
      if(ti != NULL)
        {
          ti->greyscale           = greyscale;
          ti->reference.type      = treasure->type;
          ti->reference.material  = treasure->material;
          ti->reference.gemstones = treasure->gemstones;

          ti->image = image_load(get_data_filename(treasure_filename(&ti->reference, "png")), true);
          image = ti->image;
          if(ti->image != NULL)
            {
              if(greyscale == true)
                image_to_greyscale(ti->image);
#ifdef WITH_OPENGL
              if(globals.opengl)
                image_to_texture(ti->image, true, false, true);
#endif
              stack_push(treasure_images, ti);
            }
          else
            {
              fprintf(stderr, "%s(): Failed to load image '%s'\n", __FUNCTION__, get_data_filename(treasure_filename(&ti->reference, "png")));
              free(ti);
            }
        }
      else
        fprintf(stderr, "%s(): Failed to allocate memory: %s\n", __FUNCTION__, strerror(errno));
    }
        
  return image;
}


#ifdef WITH_OPENGL
struct td_object * gfx_td_object_treasure(struct treasure * treasure)
{
  struct td_object * object;

  object = NULL;
  assert(treasure_td_objects != NULL);
  for(unsigned int i = 0; object == NULL && i < treasure_td_objects->size; i++)
    {
      struct treasure_td_object * tto;

      tto = treasure_td_objects->data[i];
      if(tto->reference.type      == treasure->type      &&
         tto->reference.material  == treasure->material  &&
         tto->reference.gemstones == treasure->gemstones    )
        object = tto->td_object;
    }

  if(object == NULL)
    {
      struct treasure_td_object * tto;

      tto = malloc(sizeof *tto);
      assert(tto != NULL);
      if(tto != NULL)
        {
          tto->reference.type      = treasure->type;
          tto->reference.material  = treasure->material;
          tto->reference.gemstones = treasure->gemstones;

          float * material1;
          float * gemstone1;

          material1 = NULL;
          gemstone1 = NULL;
          switch(treasure->material)
            {
            case TREASURE_MATERIAL_BRONZE:  material1 = MAT_BRONZE; break;
            case TREASURE_MATERIAL_COPPER:  material1 = MAT_COPPER; break;
            case TREASURE_MATERIAL_GOLD:    material1 = MAT_GOLD;   break;
            case TREASURE_MATERIAL_SILVER:  material1 = MAT_SILVER; break;
            case TREASURE_MATERIAL_SIZEOF_: assert(0);              break;
            }
          switch(treasure->gemstones)
            {
            case TREASURE_GEMSTONE_NONE:                                   break;
            case TREASURE_GEMSTONE_AMBER:     gemstone1 = GSMAT_AMBER;     break;
            case TREASURE_GEMSTONE_AMETHYST:  gemstone1 = GSMAT_AMETHYST;  break;
            case TREASURE_GEMSTONE_DIAMOND:   gemstone1 = GSMAT_DIAMOND;   break;
            case TREASURE_GEMSTONE_EMERALD:   gemstone1 = GSMAT_EMERALD;   break;
            case TREASURE_GEMSTONE_JADE:      gemstone1 = GSMAT_JADE;      break;
            case TREASURE_GEMSTONE_OPAL:      gemstone1 = GSMAT_OPAL;      break;
            case TREASURE_GEMSTONE_RUBY:      gemstone1 = GSMAT_RUBY;      break;
            case TREASURE_GEMSTONE_SAPPHIRE:  gemstone1 = GSMAT_SAPPHIRE;  break;
            case TREASURE_GEMSTONE_TURQUOISE: gemstone1 = GSMAT_TURQUOISE; break;
            case TREASURE_GEMSTONE_SIZEOF_:   assert(0);                   break;
            }
          assert(material1 != NULL);
          tto->td_object = td_object_load(get_data_filename(treasure_filename(&tto->reference, "obj")), material1, gemstone1);
          object = tto->td_object;
          if(tto->td_object != NULL)
            {
              stack_push(treasure_td_objects, tto);
            }
          else
            {
              fprintf(stderr, "%s(): Failed to load image '%s'\n", __FUNCTION__, get_data_filename(treasure_filename(&tto->reference, "obj")));
              free(tto);
            }
        }
      else
        fprintf(stderr, "%s(): Failed to allocate memory: %s\n", __FUNCTION__, strerror(errno));
    }
        
  return object;
}
#endif
