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
#include "globals.h"
#include "ui.h"
#include "themes.h"
#include "gc.h"
#include "stack.h"

#include <errno.h>
#include <string.h>

void settings_write(void)
{ /* write settings */
  if(globals.write_settings == true)
    {
      FILE * fp;
      const char * filename;

      filename = get_save_filename("settings.json");

      fp = fopen(filename, "w");
      if(fp != NULL)
        {
          char linebuf[1024];
          bool ok;

          ok = true;

          if(ok == true)
            {
              snprintf(linebuf, sizeof linebuf, "{ \"Fullscreen\": %s,\n", globals.fullscreen ? "true" : "false");
              if(fwrite(linebuf, strlen(linebuf), 1, fp) != 1)
                ok = false;
            }

          if(ok == true && globals.locale_save == true && globals.locale != NULL)
            {
              snprintf(linebuf, sizeof linebuf, "  \"Locale\": \"%s\",\n", globals.locale);
              if(fwrite(linebuf, strlen(linebuf), 1, fp) != 1)
                ok = false;
            }          
          
          if(ok == true)
            {
              snprintf(linebuf, sizeof linebuf, "  \"OpenGL\": %s,\n  \"OpenGL_POT\": %s,\n  \"OpenGL_compressed_textures\": %s,\n",
                       globals.opengl                       ? "true" : "false",
                       globals.opengl_power_of_two_textures ? "true" : "false",
                       globals.opengl_compressed_textures   ? "true" : "false");
              if(fwrite(linebuf, strlen(linebuf), 1, fp) != 1)
                ok = false;
            }

          if(ok == true)
            {
              snprintf(linebuf, sizeof linebuf, "  \"SFX\": %s,\n  \"Music\": %s,\n  \"SFX_max_channels\": %d,\n  \"Sound_volume\": %u,\n",
                       globals.use_sfx   ? "true" : "false",
                       globals.use_music ? "true" : "false",
                       globals.max_channels,
                       globals.volume);
              if(fwrite(linebuf, strlen(linebuf), 1, fp) != 1)
                ok = false;
            }

          if(ok == true)
            {
              snprintf(linebuf, sizeof linebuf, "  \"Joysticks\": %s,\n", globals.use_joysticks ? "true" : "false");
              if(fwrite(linebuf, strlen(linebuf), 1, fp) != 1)
                ok = false;
            }

          if(ok == true)
            {
              snprintf(linebuf, sizeof linebuf, "  \"CaveSelection\": \"%s\",\n  \"CaveSelectionLevel\": %d,\n", globals.cave_selection, (int) globals.level_selection);
              if(fwrite(linebuf, strlen(linebuf), 1, fp) != 1)
                ok = false;
            }
          
          if(ok == true)
            {
              snprintf(linebuf, sizeof linebuf, "  \"ShowFPS\": %s,\n", globals.fps_counter_active == true ? "true" : "false");
              if(fwrite(linebuf, strlen(linebuf), 1, fp) != 1)
                ok = false;
            }

          if(ok == true)
            {
              char modename[32];

              switch(globals.title_game_mode)
                {
                case GAME_MODE_CLASSIC:      strcpy(modename, "classic");   break;
                case GAME_MODE_ADVENTURE:    strcpy(modename, "adventure"); break;
                case GAME_MODE_PYJAMA_PARTY: strcpy(modename, "pyjama_party"); break;
                }
              snprintf(linebuf, sizeof linebuf, "  \"GameMode\": \"%s\",\n  \"IronGirlMode\": %s,\n", modename, globals.iron_girl_mode ? "true" : "false");
              if(fwrite(linebuf, strlen(linebuf), 1, fp) != 1)
                ok = false;
            }

          if(ok == true)
            {
              snprintf(linebuf, sizeof linebuf, "  \"ActiveQuestline\": %u,\n", (unsigned int) globals.active_questline);
              if(fwrite(linebuf, strlen(linebuf), 1, fp) != 1)
                ok = false;
            }
          
          if(ok == true)
            {
              snprintf(linebuf, sizeof linebuf, "  \"TitleMidarea\": \"%s\",\n", globals.title_midarea == 0 ? "arcade" : "quests");
              if(fwrite(linebuf, strlen(linebuf), 1, fp) != 1)
                ok = false;
            }

          if(ok == true)
            {
              snprintf(linebuf, sizeof linebuf, "  \"MapTilting\": %s,\n", globals.map_tilting == true ? "true" : "false");
              if(fwrite(linebuf, strlen(linebuf), 1, fp) != 1)
                ok = false;
            }

          if(ok == true)
            {
              snprintf(linebuf, sizeof linebuf, "  \"SmoothClassicMode\": %s,\n", globals.smooth_classic_mode == true ? "true" : "false");
              if(fwrite(linebuf, strlen(linebuf), 1, fp) != 1)
                ok = false;
            }
          

          /* Bindings */
          struct ui_binding ** bindings;
          bool first;
      
          if(ok == true)
            {
              snprintf(linebuf, sizeof linebuf, "  \"Bindings\": [\n");
              if(fwrite(linebuf, strlen(linebuf), 1, fp) != 1)
                ok = false;
            }

          first = true;
          bindings = ui_get_bindings(-1);
          for(int i = 0; ok == true && bindings[i] != NULL; i++)
            if(bindings[i]->editable == true)
              {
                bool write_it;
                char * s_command;

                s_command = ui_command_name(bindings[i]->command);

                write_it = false;
                switch(bindings[i]->event.type)
                  {
                  case SDL_KEYDOWN:
                    snprintf(linebuf, sizeof linebuf, "    { \"Type\": \"key\", \"Command\": \"%s\", \"Key\": \"%s\", \"Modifiers\": \"%s\" }",
                             s_command,
                             ui_keyboard_key_name(bindings[i]->event.key.keysym.sym),
                             ui_keyboard_modifier_name(bindings[i]->event.key.keysym.mod));
                    write_it = true;
                    break;
                  case SDL_JOYAXISMOTION:
                    snprintf(linebuf, sizeof linebuf, "    { \"Type\": \"joystick_axis\", \"Command\": \"%s\", \"Joystick\": %d, \"Axis\": %d, \"Value\": %d }",
                             s_command,
                             (int) bindings[i]->event.jaxis.which,
                             (int) bindings[i]->event.jaxis.axis,
                             (int) bindings[i]->event.jaxis.value);
                    write_it = true;
                    break;
                  case SDL_JOYBUTTONDOWN:
                    snprintf(linebuf, sizeof linebuf, "    { \"Type\": \"joystick_button\", \"Command\": \"%s\", \"Joystick\": %d, \"Button\": %d }",
                             s_command,
                             (int) bindings[i]->event.jbutton.which,
                             (int) bindings[i]->event.jbutton.button);
                    write_it = true;
                    break;
                  default:
                    break;
                  }

                if(write_it == true)
                  {
                    if(first == true)
                      first = false;
                    else
                      {
                        char tmp[] = ",\n";
                        if(fwrite(tmp, strlen(tmp), 1, fp) != 1)
                          ok = false;
                      }

                    if(ok == true)
                      if(fwrite(linebuf, strlen(linebuf), 1, fp) != 1)
                        ok = false;
                  }
              }
          free(bindings);
          bindings = NULL;

          if(ok == true)
            {
              snprintf(linebuf, sizeof linebuf, "\n  ],\n");
              if(fwrite(linebuf, strlen(linebuf), 1, fp) != 1)
                ok = false;
            }


          /* Themes */
          struct stack * themes;
          
          if(ok == true)
            {
              snprintf(linebuf, sizeof linebuf, "  \"Themes\": [\n");
              if(fwrite(linebuf, strlen(linebuf), 1, fp) != 1)
                ok = false;
            }

          themes = gc_get_stack(GCT_THEME);
          first = true;
          for(unsigned int i = 0; ok == true && i < themes->size; i++)
            {
              struct theme * theme;
              
              theme = themes->data[i];
              if(strcmp(theme->name, "default"))
                {
                  if(first == true)
                    first = false;
                  else
                    {
                      char tmp[] = ",\n";
                      if(fwrite(tmp, strlen(tmp), 1, fp) != 1)
                        ok = false;
                    }

                  snprintf(linebuf, sizeof linebuf, "    { \"Name\": \"%s\", \"Enabled\": %s }",
                           theme->name,
                           theme->enabled == true ? "true" : "false");
                  if(fwrite(linebuf, strlen(linebuf), 1, fp) != 1)
                    ok = false;
                }
            }

          if(ok == true)
            {
              snprintf(linebuf, sizeof linebuf, "\n  ]\n}\n");
              if(fwrite(linebuf, strlen(linebuf), 1, fp) != 1)
                ok = false;
            }


          if(ok == false)
            fprintf(stderr, "Failed to save settings into '%s': %s\n", filename, strerror(errno));

          if(fclose(fp) != 0)
            fprintf(stderr, "Failed to save settings into '%s': %s\n", filename, strerror(errno));
        }
      else
        fprintf(stderr, "Failed to save settings into '%s': %s\n", filename, strerror(errno));
    }
}
