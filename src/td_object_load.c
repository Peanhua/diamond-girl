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

#ifdef WITH_OPENGL

#include "diamond_girl.h"
#include "td_object.h"
#include "globals.h"
#include "image.h"
#include "gfx.h"
#include "texture.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef LIB3DS_V1
# include <lib3ds/file.h>
# include <lib3ds/material.h>
# include <lib3ds/matrix.h>
# include <lib3ds/mesh.h>
#else
# include <lib3ds.h>
#endif


static unsigned int calculate_face_count(struct Lib3dsFile * fp, char const * const meshname);
static unsigned int load_mesh(struct td_object * object, unsigned int pos, char const * const meshname, struct Lib3dsFile * fp, struct Lib3dsMesh * mesh);
static void         load_face(struct td_object * object, unsigned int pos, struct Lib3dsFile * fp, struct Lib3dsMesh * mesh, struct Lib3dsFace * face);



struct td_object * td_object_load(char const * const filename, char const * const meshname)
{
  struct td_object * rv;

  rv = malloc(sizeof *rv);
  assert(rv != NULL);
  if(rv != NULL)
    {
      struct Lib3dsFile * fp;

      rv->vbo_vertex              = 0;
      rv->vbo_normal              = 0;
      rv->vbo_texture_coordinates = 0;
      rv->face_count              = 0;
      rv->vertices                = NULL;
      rv->normals                 = NULL;
      rv->colours                 = NULL;
      rv->texture_coordinates     = NULL;
      rv->texture_image           = NULL;

#ifdef LIB3DS_V1
      fp = lib3ds_file_load(filename);
#else
      fp = lib3ds_file_open(filename);
#endif
      assert(fp != NULL);
      if(fp != NULL)
        {
          rv->face_count = calculate_face_count(fp, meshname);

          if(rv->face_count > 0)
            {
              rv->vertices             = malloc(sizeof(GLfloat) * 3 * rv->face_count * 3);
              rv->normals              = malloc(sizeof(GLfloat) * 3 * rv->face_count * 3);
              rv->colours              = malloc(sizeof(GLfloat) * 3 * rv->face_count * 3);
              rv->texture_coordinates  = malloc(sizeof(GLfloat) * 2 * rv->face_count * 3);

              if(rv->vertices != NULL && rv->normals != NULL && rv->colours != NULL)
                {
                  unsigned int pos;
                  
                  pos = 0;
#ifdef LIB3DS_V1
                  for(struct Lib3dsMesh * mesh = fp->meshes; mesh != NULL; mesh = mesh->next)
                    pos += load_mesh(rv, pos, meshname, fp, mesh);
#else
                  for(int mi = 0; mi < fp->nmeshes; mi++)
                    {
                      struct Lib3dsMesh * mesh = fp->meshes[mi];
                      pos += load_mesh(rv, pos, meshname, fp, mesh);
                    }
#endif

                  if(pos > 0)
                    {
                      /* Generate VBO's, generate vbo's also for pre-1.5 if the ARB extension is available */
#ifdef LIB3DS_V1
                      assert(sizeof(Lib3dsVector) == sizeof(GLfloat) * 3);
#endif
                      if(globals.opengl_1_5)
                        {
                          glGenBuffers(1, &rv->vbo_vertex);
                          glBindBuffer(GL_ARRAY_BUFFER, rv->vbo_vertex);
                          glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * 3 * rv->face_count, rv->vertices, GL_STATIC_DRAW);
          
                          glGenBuffers(1, &rv->vbo_normal);
                          glBindBuffer(GL_ARRAY_BUFFER, rv->vbo_normal);
                          glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * 3 * rv->face_count, rv->normals, GL_STATIC_DRAW);

                          glGenBuffers(1, &rv->vbo_colour);
                          glBindBuffer(GL_ARRAY_BUFFER, rv->vbo_colour);
                          glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * 3 * rv->face_count, rv->colours, GL_STATIC_DRAW);

                          glGenBuffers(1, &rv->vbo_texture_coordinates);
                          glBindBuffer(GL_ARRAY_BUFFER, rv->vbo_texture_coordinates);
                          glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * 2 * rv->face_count, rv->texture_coordinates, GL_STATIC_DRAW);
                        }
                      else if(GLEW_ARB_vertex_buffer_object)
                        {
                          glGenBuffersARB(1, &rv->vbo_vertex);
                          glBindBufferARB(GL_ARRAY_BUFFER_ARB, rv->vbo_vertex);
                          glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(GLfloat) * 3 * 3 * rv->face_count, rv->vertices, GL_STATIC_DRAW_ARB);
          
                          glGenBuffersARB(1, &rv->vbo_normal);
                          glBindBufferARB(GL_ARRAY_BUFFER_ARB, rv->vbo_normal);
                          glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(GLfloat) * 3 * 3 * rv->face_count, rv->normals, GL_STATIC_DRAW_ARB);

                          glGenBuffersARB(1, &rv->vbo_colour);
                          glBindBufferARB(GL_ARRAY_BUFFER_ARB, rv->vbo_colour);
                          glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(GLfloat) * 3 * 3 * rv->face_count, rv->colours, GL_STATIC_DRAW_ARB);

                          glGenBuffersARB(1, &rv->vbo_texture_coordinates);
                          glBindBufferARB(GL_ARRAY_BUFFER_ARB, rv->vbo_texture_coordinates);
                          glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(GLfloat) * 3 * 2 * rv->face_count, rv->texture_coordinates, GL_STATIC_DRAW_ARB);
                        }
                    }
                  else
                    {
                      fprintf(stderr, "Failed to load mesh '%s' from '%s'.\n", meshname, filename);
                      rv = td_object_unload(rv);
                    }
                }
              else
                {
                  fprintf(stderr, "%s(): Failed to allocate memory.\n", __FUNCTION__);
                  rv = td_object_unload(rv);
                }

              lib3ds_file_free(fp);
              fp = NULL;
            }
          else
            {
              fprintf(stderr, "Failed to load '%s': no faces\n", filename);
              rv = td_object_unload(rv);
            }
        }
      else
        {
          fprintf(stderr, "Failed to open '%s'.\n", filename);
          rv = td_object_unload(rv);
        }
    }
  else
    fprintf(stderr, "Failed to allocate memory: %s\n", strerror(errno));

  return rv;
}





static unsigned int calculate_face_count(struct Lib3dsFile * fp, char const * const meshname)
{
  unsigned int rv;

  rv = 0;
#ifdef LIB3DS_V1
  for(struct Lib3dsMesh * mesh = fp->meshes; mesh != NULL; mesh = mesh->next)
    if(meshname == NULL || !strcmp(meshname, mesh->name))
      rv += mesh->faces;
#else
  for(int i = 0; i < fp->nmeshes; i++)
    if(meshname == NULL || !strcmp(meshname, fp->meshes[i]->name))
      rv += fp->meshes[i]->nfaces;
#endif

  return rv;
}

static void load_face(struct td_object * object, unsigned int pos, struct Lib3dsFile * fp, struct Lib3dsMesh * mesh, struct Lib3dsFace * face)
{
  /* Vertices */
#ifdef LIB3DS_V1
  for(int i = 0; i < 3; i++)
    for(int j = 0; j < 3; j++)
      object->vertices[pos * 3 * 3 + i * 3 + j] = mesh->pointL[face->points[i]].pos[j];
#else
  for(int i = 0; i < 3; i++)
    for(int j = 0; j < 3; j++)
      object->vertices[pos * 3 * 3 + i * 3 + j] = mesh->vertices[face->index[i]][j];
#endif

  /* Texture coordinates */
#ifdef LIB3DS_V1
  if(mesh->texels > 0)
    for(int i = 0; i < 3; i++)
      for(int j = 0; j < 2; j++)
        object->texture_coordinates[pos * 3 * 2 + i * 2 + j] = mesh->texelL[face->points[i]][j];
#else
  if(mesh->texcos)
    for(int i = 0; i < 3; i++)
      for(int j = 0; j < 2; j++)
        object->texture_coordinates[pos * 3 * 2 + i * 2 + j] = mesh->texcos[face->index[i]][j];
#endif
                            

  struct Lib3dsMaterial * material;

  material = NULL;
#ifdef LIB3DS_V1
  for(Lib3dsMaterial * mat = fp->materials; material == NULL && mat != NULL; mat = mat->next)
    if(!strcmp(face->material, mat->name))
      material = mat;
#else
  material = fp->materials[face->material];
#endif

  /* Colours */
  assert(material != NULL);
  if(material != NULL)
    for(unsigned int i = 0; i < 3; i++)
      for(unsigned int j = 0; j < 3; j++)
        object->colours[pos * 3 * 3 + i * 3 + j] = material->diffuse[j];

  /* Load texture (single texture per td_object). */
  if(object->texture_image == NULL && strlen(material->texture1_map.name) > 0)
    {
      char fn[1024];
      
      snprintf(fn, sizeof fn, "gfx/%s", material->texture1_map.name);
      object->texture_image = image_load(get_data_filename(fn), false);
      if(object->texture_image != NULL)
        {
          texture_setup_from_image(object->texture_image, false, false, true);
          GFX_GL_ERROR();
        }
    }
}


static unsigned int load_mesh(struct td_object * object, unsigned int pos, char const * const meshname, struct Lib3dsFile * fp, struct Lib3dsMesh * mesh)
{
  if(meshname == NULL || !strcmp(meshname, mesh->name))
    {
      /* Normals */
#ifdef LIB3DS_V1
      lib3ds_mesh_calculate_normals(mesh, (Lib3dsVector *) (object->normals + pos * 3 * 3));
#else
      lib3ds_mesh_calculate_vertex_normals(mesh, (float(*)[3])(object->normals + pos * 3 * 3));
#endif
                
      /* Vertices, colours, and texture coordinates */
#ifdef LIB3DS_V1
      for(unsigned int cur_face = 0; cur_face < mesh->faces; cur_face++)
        {
          struct Lib3dsFace * face;
                    
          face = &mesh->faceL[cur_face];
          load_face(object, pos, fp, mesh, face);

          pos++;
          assert(pos <= object->face_count);
        }
#else
      for(int fi = 0; fi < mesh->nfaces; fi++)
        {
          struct Lib3dsFace * face;
          
          face = &mesh->faces[fi];
          load_face(object, pos, fp, mesh, face);

          pos++;
          assert(pos <= object->face_count);
        }            
#endif

      /* Transformation matrix */
      for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
          object->matrix[i * 4 + j] = mesh->matrix[i][j];
    }

  return pos;
}


#endif
