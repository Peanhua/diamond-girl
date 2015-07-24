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

bool parse_commandline_arguments(int argc, char * argv[])
{
  bool rv, print_usage;

  rv          = true;
  print_usage = false;

  for(int i = 1; i < argc; i++)
    {
      if(!strcmp(argv[i],      "--no-sound")        || !strcmp(argv[i], "-s"))
        {
          globals.use_sfx   = false;
          globals.use_music = false;
        }
      else if(!strcmp(argv[i], "--sound")           || !strcmp(argv[i], "-S"))
        {
          globals.use_sfx   = true;
          globals.use_music = true;
        }
      else if(!strcmp(argv[i], "--no-joysticks")    || !strcmp(argv[i], "-j"))
        globals.use_joysticks = false;
      else if(!strcmp(argv[i], "--joysticks")       || !strcmp(argv[i], "-J"))
        globals.use_joysticks = true;
      else if(!strcmp(argv[i], "--fullscreen")      || !strcmp(argv[i], "-f"))
	globals.fullscreen = true;
      else if(!strcmp(argv[i], "--windowed")        || !strcmp(argv[i], "-w"))
	globals.fullscreen = false;
#ifdef WITH_OPENGL
      else if(!strcmp(argv[i], "--opengl")          || !strcmp(argv[i], "-o"))
        globals.opengl = true;
      else if(!strcmp(argv[i], "--disable-shaders"))
        globals.opengl_disable_shaders = true;
#endif
      else if(!strcmp(argv[i], "--no-opengl")       || !strcmp(argv[i], "-n"))
        globals.opengl = false;
      else if(!strncmp(argv[i], "--language=", strlen("--language=")) && strlen(argv[i]) > strlen("--language="))
        {
          struct
          {
            char * name;
            char * locale;
          }
          langs[] =
            {
              { "en", "en_US" },
              { "fi", "fi_FI" },
              { "fr", "fr_BE" },
              { NULL, NULL    }
            };

          char * l;

          l = NULL;
          for(int j = 0; l == NULL && langs[j].name != NULL; j++)
            if(!strcmp(langs[j].name, argv[i] + strlen("--language=")))
               l = langs[j].locale;
          if(l != NULL)
            {
              set_language(l);
              globals.locale_save = true;
            }
          else
            {
              fprintf(stderr, "%s: Invalid language '%s' for --language.\n", argv[0], argv[i] + strlen("--language="));
              rv = false;
              print_usage = true;
            }
        }
      else if(!strcmp(argv[i], "--capture")         || !strcmp(argv[i], "-c"))
        globals.screen_capture_path = strdup("./");
      else if(!strncmp(argv[i], "--capture-frameskip=", strlen("--capture-frameskip=")))
        globals.screen_capture_frameskip = strtol(argv[i] + strlen("--capture-frameskip="), NULL, 0);
      else if(!strcmp(argv[i], "--no-swap-control") || !strcmp(argv[i], "-C"))
        globals.opengl_swap_control = false;
      else if(!strcmp(argv[i], "--show-fps")        || !strcmp(argv[i], "-F"))
        globals.fps_counter_active = true;
      else if(!strcmp(argv[i], "--no-show-fps")     || !strcmp(argv[i], "-N"))
        globals.fps_counter_active = false;
      else if(!strncmp(argv[i], "--playback=", strlen("--playback=")) && strlen(argv[i]) > strlen("--playback="))
        {
          snprintf(globals.playback_name, sizeof globals.playback_name, "%s", argv[i] + strlen("--playback="));
          globals.skip_intro = true;
        }
      else if(!strncmp(argv[i], "--play-sfx=", strlen("--play-sfx=")) && strlen(argv[i]) > strlen("--play-sfx="))
        snprintf(globals.play_sfx_name, sizeof globals.play_sfx_name, "%s", argv[i] + strlen("--play-sfx="));
      else if(!strcmp(argv[i], "--read-only"))
        globals.read_only = true;
#ifndef NDEBUG
      else if(!strcmp(argv[i], "--backtrace"))
        globals.backtrace = true;
      else if(!strcmp(argv[i], "--quit"))
        globals.quit_after_initialization = true;
#endif
      else if(!strcmp(argv[i], "--skip-intro"))
        globals.skip_intro = true;
      else if(!strcmp(argv[i], "--help")            || !strcmp(argv[i], "-h"))
	{
	  rv = false;
	  print_usage = true;
	}
      else
	{
	  fprintf(stderr, "%s: Invalid argument %d '%s'.\n", argv[0], (int) i, argv[i]);
	  rv = false;
	  print_usage = true;
	}
    }

  if(print_usage == true)
    {
      printf("Lucy the Diamond Girl v%s\n", VERSION);
      printf("Usage: %s [OPTION]...\n", argc > 0 ? argv[0] : "Lucy the Diamond Girl");
      printf("  -h | --help             Print this help text and exit.\n");
      printf("  -s | --no-sound         Disable sounds and music. (*)\n");
      printf("  -S | --sound            Enable sounds and music. (*)\n");
      printf("  -j | --no-joysticks     Disable joysticks. (*)\n");
      printf("  -J | --joysticks        Enable joysticks. (*)\n");
      printf("  -f | --fullscreen       Fullscreen mode. (*)\n");
      printf("  -w | --windowed         Windowed mode. (*)\n");
#ifdef WITH_OPENGL
      printf("  -o | --opengl           OpenGL mode. (*)\n");
      printf("       --disable-shaders  Use the fixed pipeline.\n");
#endif
#ifdef WITH_NONOPENGL
      printf("  -n | --no-opengl        Non-OpenGL mode. (*)\n");
#endif
      printf("  --language=NAME         Set the user interface language to NAME. (*)\n");
      printf("                          Possible values for NAME are: en, fi, fr\n");
      printf("  -F | --show-fps         Output FPS every 1000th frame to stdout (console). (*)\n");
      printf("  -N | --no-show-fps      Don't output FPS every 1000th frame to stdout. (*)\n");
      printf("  -c | --capture          Capture each frame into the current directory as a bmp file.\n");
      printf("  --capture-frameskip=N   Capture every Nth frame.\n");
      printf("  -C | --no-swap-control  Disable OpenGL swap control, may allow faster FPS than the refresh rate of the monitor is.\n");
      printf("  --playback=NAME         Playback NAME and then exit. The name can be seen when manually playbacking a game.\n");
      printf("  --play-sfx=NAME         Play sound effect NAME, and then exit.\n");
      printf("  --skip-intro            Skip the intro screen (visible in OpenGL mode only).\n");
      printf("  --read-only             Run the game in read-only mode: don't write any files.\n");
#ifndef NDEBUG
      printf("  --backtrace             Generate debugging backtraces.\n");
      printf("  --quit                  Quit after initialization.\n");
#endif
      printf("Options marked with (*) are saved as your configuration in '%s'.\n", get_save_filename("settings.json"));
    }


  if(rv == true)
    {
      if(globals.screen_capture_path != NULL)
        {
          if(globals.opengl)
            {
              printf("Capturing enabled:\n");
              printf(" saving under: %s\n", globals.screen_capture_path);
              printf(" frameskip   : %d (capture every %d frame)\n", globals.screen_capture_frameskip, globals.screen_capture_frameskip);
            }
          else
            printf("Capturing DISABLED: this feature requires OpenGL\n");
        }
      if(strlen(globals.playback_name) > 0)
        printf("Playbacking '%s'.\n", globals.playback_name);
    }
      
  return rv;
}
