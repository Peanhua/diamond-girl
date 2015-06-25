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
#include "td_object.h"
#include "globals.h"
#include "image.h"
#include "gfx.h"
#include "gfxbuf.h"
#include "stack.h"
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


static struct td_object_mesh * load_mesh(struct Lib3dsFile * fp, struct Lib3dsMesh * mesh);
static void                    load_face(struct td_object_mesh * tdm, struct Lib3dsFile * fp, struct Lib3dsMesh * mesh, struct Lib3dsFace * face);
static void                    copy_lib3ds_matrix(struct td_object_mesh * tdm, Lib3dsMatrix matrix);


struct td_object * td_object_load(char const * const filename)
{
  struct td_object * rv;

  rv = malloc(sizeof *rv);
  assert(rv != NULL);
  if(rv != NULL)
    {
#ifndef NDEBUG
      rv->filename = strdup(filename);
#endif
      rv->meshes = stack_new();
      if(rv->meshes != NULL)
        {
          struct Lib3dsFile * fp;

#ifdef LIB3DS_V1
          fp = lib3ds_file_load(filename);
#else
          fp = lib3ds_file_open(filename);
#endif
          assert(fp != NULL);
          if(fp != NULL)
            {
#ifdef LIB3DS_V1
              for(struct Lib3dsMesh * mesh = fp->meshes; rv != NULL && mesh != NULL; mesh = mesh->next)
                {
                  struct td_object_mesh * tdm;
                  
                  tdm = load_mesh(fp, mesh);
                  assert(tdm != NULL);
                  if(tdm != NULL)
                    stack_push(rv->meshes, tdm);
                  else
                    {
                      fprintf(stderr, "%s: Failed to load mesh '%s' from '%s'.\n", __FUNCTION__, mesh->name, filename);
                      rv = td_object_unload(rv);
                    }
                }
#else
              for(int mi = 0; mi < fp->nmeshes; mi++)
                {
                  struct td_object_mesh * tdm;
                  
                  tdm = load_mesh(fp, fp->meshes[mi]);
                  assert(tdm != NULL);
                  if(tdm != NULL)
                    stack_push(rv->meshes, tdm);
                  else
                    {
                      fprintf(stderr, "%s: Failed to load mesh '%s' from '%s'.\n", __FUNCTION__, mesh->name, filename);
                      rv = td_object_unload(rv);
                    }
                }
#endif
              lib3ds_file_free(fp);
              fp = NULL;

              if(rv != NULL)
                { /* Optimize by merging similar meshes together. */
                  bool more;

                  more = true;
                  while(more == true)
                    {
                      more = false;
                      for(unsigned int i = 0; more == false && i + 1 < rv->meshes->size; i++)
                        for(unsigned int j = i + 1; more == false && j < rv->meshes->size; j++)
                          {
                            struct td_object_mesh * dst, * src;
                            
                            dst = rv->meshes->data[i];
                            src = rv->meshes->data[j];
                            
                            if(dst->texture_image == NULL)
                              if(gfxbuf_merge(dst->gfxbuf, src->gfxbuf) == true)
                                {
                                  stack_pull(rv->meshes, src);
                                  gfxbuf_free(src->gfxbuf);
                                  free(src);
                                  
                                  gfxbuf_update(dst->gfxbuf, 0, dst->gfxbuf->vertices);
                                  more = true;
                                }
                          }
                    }
                }
            }
          else
            {
              fprintf(stderr, "Failed to open '%s'.\n", filename);
              rv = td_object_unload(rv);
            }
        }
      else
        {
          fprintf(stderr, "%s: Failed to allocate stack.\n", __FUNCTION__);
          rv = td_object_unload(rv);
        }
    }
  else
    fprintf(stderr, "%s: Failed to allocate memory: %s\n", __FUNCTION__, strerror(errno));


  return rv;
}






static void load_face(struct td_object_mesh * tdm, struct Lib3dsFile * fp, struct Lib3dsMesh * mesh, struct Lib3dsFace * face)
{
  assert(tdm->gfxbuf->vertices + 3 <= tdm->gfxbuf->max_vertices);

  /* Vertices */
#ifdef LIB3DS_V1
  for(int i = 0; i < 3; i++)
    for(int j = 0; j < 3; j++)
      tdm->gfxbuf->vbuf[tdm->gfxbuf->vertices * 3 + i * 3 + j] = mesh->pointL[face->points[i]].pos[j];
#else
  for(int i = 0; i < 3; i++)
    for(int j = 0; j < 3; j++)
      tdm->gfxbuf->vbuf[tdm->gfxbuf->vertices * 3 + i * 3 + j] = mesh->vertices[face->index[i]][j];
#endif

  /* Texture coordinates */
#ifdef LIB3DS_V1
  if(mesh->texels > 0)
    for(int i = 0; i < 3; i++)
      for(int j = 0; j < 2; j++)
        tdm->gfxbuf->tbuf[tdm->gfxbuf->vertices * 2 + i * 2 + j] = mesh->texelL[face->points[i]][j];
#else
  if(mesh->texcos)
    for(int i = 0; i < 3; i++)
      for(int j = 0; j < 2; j++)
        tdm->gfxbuf->tbuf[tdm->gfxbuf->vertices * 2 + i * 2 + j] = mesh->texcos[face->index[i]][j];
#endif
                            

  struct Lib3dsMaterial * material; // move this to load_mesh() because we're assuming single material per mesh ?

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
      for(unsigned int j = 0; j < 4; j++)
        tdm->gfxbuf->cbuf[tdm->gfxbuf->vertices * 4 + i * 4 + j] = material->diffuse[j] * 255.0;
  
  /* Load texture (single texture per td_object). */
  if(tdm->texture_image == NULL && strlen(material->texture1_map.name) > 0)
    {
      char fn[1024];
      
      snprintf(fn, sizeof fn, "gfx/%s", material->texture1_map.name);
      tdm->texture_image = image_load(get_data_filename(fn), false);
      if(tdm->texture_image != NULL)
        {
          texture_setup_from_image(tdm->texture_image, false, false, true);
          GFX_GL_ERROR();
        }
    }

  tdm->gfxbuf->vertices += 3;
}


static struct td_object_mesh * load_mesh(struct Lib3dsFile * fp, struct Lib3dsMesh * mesh)
{
  struct td_object_mesh * rv;

  rv = malloc(sizeof *rv);
  if(rv != NULL)
    {
      Lib3dsMatrix identity;
      
      rv->texture_image = NULL;

      lib3ds_matrix_identity(identity);
      copy_lib3ds_matrix(rv, identity);
  
      unsigned int face_count;
#ifdef LIB3DS_V1
      face_count = mesh->faces;
#else
      face_count = fp->meshes[i]->nfaces;
#endif

      bool has_texture;

      has_texture = false;
#ifdef LIB3DS_V1
      if(mesh->texels > 0)
        has_texture = true;
#else
      if(mesh->texcos)
        has_texture = true;
#endif

      if(has_texture)
        rv->gfxbuf = gfxbuf_new(GFXBUF_STATIC_3D, GL_TRIANGLES, GFXBUF_COLOUR | GFXBUF_NORMALS | GFXBUF_TEXTURE);
      else
        rv->gfxbuf = gfxbuf_new(GFXBUF_STATIC_3D, GL_TRIANGLES, GFXBUF_COLOUR | GFXBUF_NORMALS);
        
      assert(rv->gfxbuf != NULL);
      if(rv->gfxbuf != NULL)
        {
          if(gfxbuf_resize(rv->gfxbuf, face_count * 3))
            {
              /* Normals */
              if(rv->gfxbuf->options & GFXBUF_NORMALS)
                {
#ifdef LIB3DS_V1
                  lib3ds_mesh_calculate_normals(mesh, (Lib3dsVector *) rv->gfxbuf->nbuf);
#else
                  lib3ds_mesh_calculate_vertex_normals(mesh, (float(*)[3]) rv->gfxbuf->nbuf);
#endif
                }
                
              /* Copy faces (vertices, colours, and texture coordinates). */
#ifdef LIB3DS_V1
              for(unsigned int cur_face = 0; cur_face < mesh->faces; cur_face++)
                {
                  struct Lib3dsFace * face;
                    
                  face = &mesh->faceL[cur_face];
                  load_face(rv, fp, mesh, face);
                }
#else
              for(unsigned int fi = 0; fi < mesh->nfaces; fi++)
                {
                  struct Lib3dsFace * face;
              
                  face = &mesh->faces[fi];
                  load_face(rv, fp, mesh, face);
                }            
#endif

              assert(rv->gfxbuf->vertices == face_count * 3);
              gfxbuf_update(rv->gfxbuf, 0, rv->gfxbuf->vertices);
              
              /* Transformation matrix */
              copy_lib3ds_matrix(rv, mesh->matrix);
            }
          else
            {
              fprintf(stderr, "%s(): Failed to allocate memory.\n", __FUNCTION__);
              // todo: free resources 
              free(rv);
              rv = NULL;
            }
        }
      else
        {
          fprintf(stderr, "%s(): Failed to allocate memory.\n", __FUNCTION__);
          // todo: free resources 
          free(rv);
          rv = NULL;
        }
    }

  return rv;
}



static void copy_lib3ds_matrix(struct td_object_mesh * tdm, Lib3dsMatrix matrix)
{
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      tdm->matrix[i * 4 + j] = matrix[i][j];
}


#endif
