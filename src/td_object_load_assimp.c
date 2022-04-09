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

#include "diamond_girl.h" // For DG_UNUSED, todo: remove
#include "gfxbuf.h"
#include "gfx_material.h"
#include "image.h"
#include "stack.h"
#include "td_object.h"
#include "texture.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct td_object_mesh * load_node(struct aiScene const * scene, struct aiNode const * node, struct aiMatrix4x4 const * matrix, float * material1, float * gemstone1);
static struct gfxbuf * load_mesh(struct aiScene const * scene, struct aiMesh const * mesh, float * material1, float * gemstone1);
static void copy_assimp_matrix(GLfloat * target, struct aiMatrix4x4 const * source);

#ifndef NDEBUG
static char * the_filename;
static bool   logdone = false;
#endif

struct td_object * td_object_load_assimp(char const * const filename, float * material1, float * gemstone1)
{
  struct td_object * rv;

#ifndef NDEBUG
  the_filename = strdup(filename);
#endif
  
  rv = malloc(sizeof *rv);
  assert(rv != NULL);
  if(rv != NULL)
    {
#ifndef NDEBUG
      rv->filename = strdup(filename);
#endif
      rv->root     = NULL;

      
#ifndef NDEBUG
      if(logdone == false)
        {
#if 0
          struct aiLogStream stream;
          
          stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT, NULL);
          aiAttachLogStream(&stream);
#endif
          logdone = true;
        }
#endif
      
      struct aiScene const * scene;
      
      scene = aiImportFile(filename, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_PreTransformVertices | aiProcess_Triangulate);
      if(scene != NULL)
        {
          struct aiMatrix4x4 m;

          aiIdentityMatrix4(&m);
          rv->root = load_node(scene, scene->mRootNode, &m, material1, gemstone1);
        }
      else
        {
          fprintf(stderr, "%s: Failed to load '%s'\n", __FUNCTION__, filename);
          rv = td_object_unload(rv);
        }
    }
  else
    fprintf(stderr, "%s: Failed to allocate memory: %s\n", __FUNCTION__, strerror(errno));

#ifndef NDEBUG
  free(the_filename);
#endif

  return rv;
}



static struct td_object_mesh * load_node(struct aiScene const * scene, struct aiNode const * node, struct aiMatrix4x4 const * matrix, float * material1, float * gemstone1)
{
  struct td_object_mesh * rv;

  rv = malloc(sizeof *rv);
  assert(rv != NULL);
  if(rv != NULL)
    {
      rv->gfxbufs = stack_new();
      rv->children = stack_new();
      
      struct aiMatrix4x4 m;

      m = node->mTransformation;
      aiMultiplyMatrix4(&m, matrix);
      copy_assimp_matrix(rv->matrix, &m);

      for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
          struct aiMesh const * mesh;
          struct gfxbuf * buf;
      
          mesh = scene->mMeshes[node->mMeshes[i]];
          buf = load_mesh(scene, mesh, material1, gemstone1);
          if(buf != NULL)
            stack_push(rv->gfxbufs, buf);
        }

      for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
          struct td_object_mesh * child;
          
          child = load_node(scene, node->mChildren[i], &m, material1, gemstone1);
          if(child != NULL)
            stack_push(rv->children, child);
        }
    }
  else
    fprintf(stderr, "%s(): Failed to allocate memory.\n", __FUNCTION__);

  return rv;
}


static struct gfxbuf * load_mesh(struct aiScene const * scene, struct aiMesh const * mesh, float * material1, float * gemstone1)
{
  struct gfxbuf * rv;
  unsigned int flags;

  flags = 0;
  if(mesh->mColors[0] != NULL)
    flags |= GFXBUF_COLOUR;
  if(mesh->mNormals != NULL)
    flags |= GFXBUF_NORMALS;
  if(mesh->mTextureCoords[0] != NULL)
    flags |= GFXBUF_TEXTURE;
  
  rv = gfxbuf_new(GFXBUF_STATIC_3D, GL_TRIANGLES, flags);
  if(rv != NULL)
    if(gfxbuf_resize(rv, mesh->mNumFaces * 3))
      {
        struct aiMaterial const * material;
        struct aiString           name;

        material = scene->mMaterials[mesh->mMaterialIndex];

        if(aiGetMaterialString(material, AI_MATKEY_NAME, &name) != AI_SUCCESS)
          snprintf(name.data, sizeof name.data, "FAILED TO OBTAIN MATERIAL NAME");

        if(!strcmp(name.data, "material1"))
          {
            if(rv->material == NULL)
              rv->material = gfx_material_new();
            
            for(int i = 0; i < 4; i++)
              rv->material->diffuse[i] = material1[i];
          }
        else if(!strcmp(name.data, "gemstone1"))
          {
            if(rv->material == NULL)
              rv->material = gfx_material_new();

            for(int i = 0; i < 4; i++)
              rv->material->diffuse[i] = gemstone1[i];
          }
        else
          {
            {
              struct aiString tfn;

              if(aiGetMaterialString(material, AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), &tfn) == AI_SUCCESS)
                {
                  char buf[strlen("gfx/") + 1024];

                  snprintf(buf, sizeof buf, "gfx/%s", tfn.data);
                  rv->texture_image = image_load(get_data_filename(buf), false);
                  if(rv->texture_image != NULL)
                    texture_setup_from_image(rv->texture_image, false, false, true);
                }
            }
        
            struct aiColor4D colour;
        
            if(aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &colour) == AI_SUCCESS)
              {
                if(rv->material == NULL)
                  rv->material = gfx_material_new();
                
                rv->material->ambient[0] = colour.r;
                rv->material->ambient[1] = colour.g;
                rv->material->ambient[2] = colour.b;
                rv->material->ambient[3] = colour.a;
              }
            if(aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &colour) == AI_SUCCESS)
              {
                if(rv->material == NULL)
                  rv->material = gfx_material_new();

                rv->material->diffuse[0] = colour.r;
                rv->material->diffuse[1] = colour.g;
                rv->material->diffuse[2] = colour.b;
                rv->material->diffuse[3] = colour.a;
              }
            if(aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &colour) == AI_SUCCESS)
              {
                if(rv->material == NULL)
                  rv->material = gfx_material_new();

                rv->material->specular[0] = colour.r;
                rv->material->specular[1] = colour.g;
                rv->material->specular[2] = colour.b;
                rv->material->specular[3] = colour.a;
              }
            if(aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &colour) == AI_SUCCESS)
              {
                if(rv->material == NULL)
                  rv->material = gfx_material_new();

                rv->material->emission[0] = colour.r;
                rv->material->emission[1] = colour.g;
                rv->material->emission[2] = colour.b;
                rv->material->emission[3] = colour.a;
              }
            {
              unsigned int max;
              float shininess;

              max = 1;
              if(aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, &max) == AI_SUCCESS)
                {
                  float strength;

                  if(rv->material == NULL)
                    rv->material = gfx_material_new();

                  max = 1;
                  if(aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS_STRENGTH, &strength, &max) == AI_SUCCESS)
                    rv->material->shininess = shininess * strength;
                  else
                    rv->material->shininess = shininess;
              
                  if(rv->material->shininess > 128)
                    {
#ifndef NDEBUG
                      printf("%s:%s(): Warning, shininess %d exceeds 128 in file '%s', material '%s'.\n", __FILE__, __FUNCTION__, (int) rv->material->shininess, the_filename, name.data);
#endif
                      rv->material->shininess = 128;
                    }
                }
            }
          }


        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
          {
            struct aiFace const * face;
            
            face = &mesh->mFaces[i];
            assert(face->mNumIndices == 3);
            for(int j = 0; j < 3; j++)
              {
                int ind;
                
                ind = face->mIndices[j];

                rv->vbuf[rv->vertices * 3 + 0] = mesh->mVertices[ind].x;
                rv->vbuf[rv->vertices * 3 + 1] = mesh->mVertices[ind].y;
                rv->vbuf[rv->vertices * 3 + 2] = mesh->mVertices[ind].z;

                if((rv->options & GFXBUF_COLOUR) && mesh->mColors[0] != NULL)
                  {
                    rv->cbuf[rv->vertices * 4 + 0] = mesh->mColors[0][ind].r;
                    rv->cbuf[rv->vertices * 4 + 1] = mesh->mColors[0][ind].g;
                    rv->cbuf[rv->vertices * 4 + 2] = mesh->mColors[0][ind].b;
                    rv->cbuf[rv->vertices * 4 + 3] = mesh->mColors[0][ind].a;
                  }

                if((rv->options & GFXBUF_NORMALS) && mesh->mNormals != NULL)
                  {
                    rv->nbuf[rv->vertices * 3 + 0] = mesh->mNormals[ind].x;
                    rv->nbuf[rv->vertices * 3 + 1] = mesh->mNormals[ind].y;
                    rv->nbuf[rv->vertices * 3 + 2] = mesh->mNormals[ind].z;
                  }

                if((rv->options & GFXBUF_TEXTURE) && mesh->mTextureCoords[0] != NULL)
                  {
                    rv->tbuf[rv->vertices * 2 + 0] = mesh->mTextureCoords[0][ind].x;
                    rv->tbuf[rv->vertices * 2 + 1] = mesh->mTextureCoords[0][ind].y;
                  }
                
                rv->vertices++;
              }
          }

        gfxbuf_update(rv, 0, rv->vertices);
      }

  return rv;
}


static void copy_assimp_matrix(GLfloat * target, struct aiMatrix4x4 const * source)
{
  int i;
  struct aiMatrix4x4 m;

  m = *source;
  aiTransposeMatrix4(&m);
  
  i = 0;
  target[i++] = m.a1;
  target[i++] = m.a2;
  target[i++] = m.a3;
  target[i++] = m.a4;
  target[i++] = m.b1;
  target[i++] = m.b2;
  target[i++] = m.b3;
  target[i++] = m.b4;
  target[i++] = m.c1;
  target[i++] = m.c2;
  target[i++] = m.c3;
  target[i++] = m.c4;
  target[i++] = m.d1;
  target[i++] = m.d2;
  target[i++] = m.d3;
  target[i++] = m.d4;
}


#endif
