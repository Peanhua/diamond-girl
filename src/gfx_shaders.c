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

#ifdef WITH_OPENGL

#include "diamond_girl.h"
#include "gfx.h"
#include "gfx_shaders.h"
#include "globals.h"
#include <assert.h>

enum SHADER_ATTRIBUTE
  {
    SHADER_ATTRIBUTE_FOG_LINEAR,
    SHADER_ATTRIBUTE_LIGHT_DIRECTIONAL,
    SHADER_ATTRIBUTE_LIGHT_POSITIONAL,
    SHADER_ATTRIBUTE_TEXTURE,
    SHADER_ATTRIBUTE_SIZEOF_
  };
typedef uint8_t shader_attribute_t;

struct shaderprogram
{
  GLuint program;
};


static struct shaderprogram * shaderprogram_new(shader_attribute_t attributes);
static struct shaderprogram * shaderprogram_free(struct shaderprogram * shader);

static bool shader_load_mem(struct shaderprogram * program, char shader_type, char const * source);
static bool shader_load_mem1(struct shaderprogram * program, char shader_type, char const * source);
static bool shader_load_mem2(struct shaderprogram * program, char shader_type, char const * source);
static bool shader_load_file(struct shaderprogram * program, char shader_type, char const * filename);
static bool append_file(char ** buffer_p, unsigned int * size_p, char const * filename);


static struct shaderprogram * shaderprograms[1<<SHADER_ATTRIBUTE_SIZEOF_];
static shader_attribute_t current_program;
static shader_attribute_t previous_program;


void gfx_shaders_initialize(void)
{
  for(unsigned int i = 0; i < (1<<SHADER_ATTRIBUTE_SIZEOF_); i++)
    shaderprograms[i] = shaderprogram_new(i);

  current_program = 0;
  previous_program = 0;
}


void gfx_shaders_cleanup(void)
{
  for(unsigned int i = 0; i < (1<<SHADER_ATTRIBUTE_SIZEOF_); i++)
    shaderprograms[i] = shaderprogram_free(shaderprograms[i]);
}


void gfx_shaders_apply(void)
{
  if(globals.opengl_shaders > 0)
    if(previous_program != current_program)
      {
        assert(shaderprograms[current_program] != NULL);
        previous_program = current_program;
        
        if(globals.opengl_shaders == 1)
          glUseProgramObjectARB(shaderprograms[current_program]->program);
        else
          glUseProgram(shaderprograms[current_program]->program);
        GFX_GL_ERROR();
      }
}



static bool shader_load_mem(struct shaderprogram * program, char shader_type, char const * source)
{
  assert(shader_type == 'f' || shader_type == 'v');

  if(globals.opengl_shaders == 1)
    return shader_load_mem1(program, shader_type, source);
  else
    return shader_load_mem2(program, shader_type, source);
}



static bool shader_load_mem1(struct shaderprogram * program, char shader_type, char const * source)
{
  bool success;
  GLhandleARB shader;

  success = false;

  if(shader_type == 'f')
    shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
  else /* if(shader_type == 'v') */
    shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
  GFX_GL_ERROR();
  
  assert(shader > 0);
  if(shader > 0)
    {
      GLchar const * sourcelines[] = { source };

      glShaderSourceARB(shader, 1, sourcelines, NULL);
      if(glGetError() == GL_NO_ERROR)
        {
          glCompileShaderARB(shader);
          if(glGetError() == GL_NO_ERROR)
            {
              GLint param;
              
              glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &param);
              if(param == GL_TRUE)
                {
                  GLenum e;
                  
                  glAttachObjectARB(program->program, shader);
                  e = glGetError();
                  if(e == GL_NO_ERROR)
                    {
                      success = true;
                    }
                  else
                    {
#ifndef NDEBUG
                      printf("Failed to attach shader: %s\n", gluErrorString(e));
#endif
                    }
                }
              else
                {
#ifndef NDEBUG
                  printf("Failed to compile shader.\n");
#endif
                }
            }
          else
            {
#ifndef NDEBUG
              printf("Failed to compile shader.\n");
#endif
            }
        }
      else
        {
#ifndef NDEBUG
          printf("Failed to load shader source.\n");
#endif
        }

#ifndef NDEBUG
      if(success == false)
        {
          printf("--- Begin shader source ---\n%s\n--- End shader source ---\n", source);
                  
          GLchar logbuf[1024 * 4];
          GLsizei logbuflen;
                  
          glGetInfoLogARB(shader, sizeof logbuf, &logbuflen, logbuf);
          if(logbuflen > 0)
            printf("--- Begin shader info log ---\n%s\n--- End shader info log ---\n", logbuf);
        }
#endif
    }
  else
    {
#ifndef NDEBUG
      printf("Failed to create shader.\n");
#endif
    }
  return success;
}


static bool shader_load_mem2(struct shaderprogram * program, char shader_type, char const * source)
{
  bool success;
  GLuint shader;

  assert(shader_type == 'f' || shader_type == 'v');
  success = false;

  if(shader_type == 'f')
    shader = glCreateShader(GL_FRAGMENT_SHADER);
  else /* if(shader_type == 'v') */
    shader = glCreateShader(GL_VERTEX_SHADER);
  GFX_GL_ERROR();
  
  assert(shader > 0);
  if(shader > 0)
    {
      GLchar const * sourcelines[] = { source };

      glShaderSource(shader, 1, sourcelines, NULL);
      if(glGetError() == GL_NO_ERROR)
        {
          glCompileShader(shader);
          if(glGetError() == GL_NO_ERROR)
            {
              GLint param;
              
              glGetShaderiv(shader, GL_COMPILE_STATUS, &param);
              if(param == GL_TRUE)
                {
                  GLenum e;
                  
                  glAttachShader(program->program, shader);
                  e = glGetError();
                  if(e == GL_NO_ERROR)
                    {
                      success = true;
                    }
                  else
                    {
#ifndef NDEBUG
                      printf("Failed to attach shader: %s\n", gluErrorString(e));
#endif
                    }
                }
              else
                {
#ifndef NDEBUG
                  printf("Failed to compile shader.\n");
#endif
                }
            }
          else
            {
#ifndef NDEBUG
              printf("Failed to compile shader.\n");
#endif
            }
        }
      else
        {
#ifndef NDEBUG
          printf("Failed to load shader source.\n");
#endif
        }

#ifndef NDEBUG
      if(success == false)
        {
          printf("--- Begin shader source ---\n%s\n--- End shader source ---\n", source);
                  
          GLchar logbuf[1024 * 4];
          GLsizei logbuflen;
                  
          glGetShaderInfoLog(shader, sizeof logbuf, &logbuflen, logbuf);
          if(logbuflen > 0)
            printf("--- Begin shader info log ---\n%s\n--- End shader info log ---\n", logbuf);
        }
#endif
    }
  else
    {
#ifndef NDEBUG
      printf("Failed to create shader.\n");
#endif
    }
  return success;
}  



static bool shader_load_file(struct shaderprogram * program, char shader_type, char const * filename)
{
  assert(program != NULL);
  assert(program->program > 0);
  assert(globals.opengl_shaders > 0);

  bool success;
  char * buf;
  int    buf_size;

  success = read_file(get_data_filename(filename), &buf, &buf_size);
  if(success == true)
    {
      char * source;

      source = malloc(buf_size + 1);
      if(source != NULL)
        {
          memcpy(source, buf, buf_size);
          source[buf_size] = '\0';

          shader_load_mem(program, shader_type, source);

          free(source);
        }
      else
        {
#ifndef NDEBUG
          printf("Failed to allocate memory for %u bytes.\n", (unsigned int) buf_size);
#endif
          success = false;
        }
      
      free(buf);
    }
  else
    printf("Failed to read file '%s'.\n", get_data_filename(filename));
  
  assert(success == true);
  
  return success;
}


static bool append_file(char ** buffer_p, unsigned int * size_p, char const * filename)
{
  bool success;
  char * buf;
  int    buf_size;
  
  success = read_file(get_data_filename(filename), &buf, &buf_size);
  if(success == true)
    {
      char * tmp;

      tmp = realloc(*buffer_p, (*size_p) + buf_size + 1);
      if(tmp != NULL)
        {
          memcpy(tmp + (*size_p), buf, buf_size);
          tmp[(*size_p) + buf_size] = '\0';
          
          *buffer_p = tmp;
          *size_p = (*size_p) + buf_size;
        }
      else
        success = false;

      free(buf);
    }
  else
    printf("Failed to read file '%s'.\n", get_data_filename(filename));
  
  assert(success == true);

  return success;
}



static struct shaderprogram * shaderprogram_new(shader_attribute_t attributes)
{
  struct shaderprogram * rv;

  rv = malloc(sizeof *rv);
  assert(rv != NULL);
  if(rv != NULL)
    {
      rv->program = 0;

      if(globals.opengl_shaders > 0)
        {
          rv->program = glCreateProgram();
          GFX_GL_ERROR();
          assert(rv->program > 0);
          if(rv->program > 0)
            {
              bool success;

              success = shader_load_file(rv, 'v', "gfx/main.vert");
              if(success == true)
                {
                  char *       buffer;
                  unsigned int buffer_size;

                  buffer      = NULL;
                  buffer_size = 0;
                  success = append_file(&buffer, &buffer_size, "gfx/header.frag");

                  if(success == true)
                    if(attributes & (1<<SHADER_ATTRIBUTE_LIGHT_DIRECTIONAL))
                      success = append_file(&buffer, &buffer_size, "gfx/light_directional.frag");

                  if(success == true)
                    if(attributes & (1<<SHADER_ATTRIBUTE_LIGHT_POSITIONAL))
                      {
                        success = append_file(&buffer, &buffer_size, "gfx/light_positional.frag");

                        if(success == true)
                          { /* Are we the only light or was there something before us? */
                            if(attributes & (1<<SHADER_ATTRIBUTE_LIGHT_DIRECTIONAL))
                              success = append_file(&buffer, &buffer_size, "gfx/light_positional_add.frag");
                            else
                              success = append_file(&buffer, &buffer_size, "gfx/light_positional_mul.frag");
                          }
                      }

                  if(success == true)
                    if(attributes & (1<<SHADER_ATTRIBUTE_FOG_LINEAR))
                      success = append_file(&buffer, &buffer_size, "gfx/fog_linear.frag");

                  if(success == true)
                    if(attributes & (1<<SHADER_ATTRIBUTE_TEXTURE))
                      success = append_file(&buffer, &buffer_size, "gfx/texture.frag");

                  if(success == true)
                    success = append_file(&buffer, &buffer_size, "gfx/footer.frag");

                  
                  if(success == true)
                    success = shader_load_mem(rv, 'f', buffer);
                  free(buffer);

                  if(success == true)
                    {
                      glLinkProgram(rv->program);
                      if(glGetError() == GL_NO_ERROR)
                        {
                          GLint ls;
                      
                          glGetProgramiv(rv->program, GL_LINK_STATUS, &ls);
                          if(ls == GL_TRUE)
                            {
                            }
                          else
                            success = false;
                        }
                      else
                        success = false;
                    }
                }
              
              assert(success == true);
            }
        }
    }

  if(rv == NULL)
    printf("Failed to load shaderprogram %u.\n", (unsigned int) attributes);
  assert(rv != NULL);

  return rv;
}


static struct shaderprogram * shaderprogram_free(struct shaderprogram * shader)
{
  if(shader != NULL)
    {
      if(shader->program > 0)
        {
          if(globals.opengl_shaders == 1)
            glDeleteObjectARB(shader->program);
          else /* if(globals.opengl_shaders == 2) */
            glDeleteProgram(shader->program);
        }
      
      free(shader);
    }

  return NULL;
}



void gfx_shaders_set_fog(GLint mode, bool enabled)
{
  assert(globals.opengl_shaders > 0);
  if(enabled == true)
    {
      if(mode == GL_LINEAR)
        current_program |= (1<<SHADER_ATTRIBUTE_FOG_LINEAR);
      else
        assert(0);
    }
  else
    {
      current_program &= ~(1<<SHADER_ATTRIBUTE_FOG_LINEAR);
    }
}



void gfx_shaders_set_lighting(bool enabled)
{
  uint8_t lights;

  assert(globals.opengl_shaders > 0);

  lights = gfxgl_light_get();
  assert(lights > 0);

  if(enabled == true)
    {
      if(lights & (1<<GFX_LIGHT_TYPE_POSITIONAL))
        current_program |= (1<<SHADER_ATTRIBUTE_LIGHT_POSITIONAL);

      if(lights & (1<<GFX_LIGHT_TYPE_DIRECTIONAL))
        current_program |= (1<<SHADER_ATTRIBUTE_LIGHT_DIRECTIONAL);
    }
  else
    {
      current_program &= ~(1<<SHADER_ATTRIBUTE_LIGHT_POSITIONAL);
      current_program &= ~(1<<SHADER_ATTRIBUTE_LIGHT_DIRECTIONAL);
    }
}

void gfx_shaders_set_texture(bool enabled)
{
  assert(globals.opengl_shaders > 0);
  if(enabled == true)
    current_program |= (1<<SHADER_ATTRIBUTE_TEXTURE);
  else
    current_program &= ~(1<<SHADER_ATTRIBUTE_TEXTURE);
}



#endif
