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

#include "image.h"
#include <assert.h>
#include <errno.h>

/*
int main(void)
{
  struct imagepacknode * root, * tmp;

  root = image_pack_new(0, 0, 100, 100);
  do {
    struct image * img;

    img = image_new(50, 20, false);
    tmp = image_pack(root, img);
    if(tmp != NULL)
      printf("packed %dx%d at %d,%d\n", (int) tmp->w, (int) tmp->h, (int) tmp->x, (int) tmp->y);

  } while(tmp != NULL);
}
*/

struct imagepacknode * image_pack_new(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
  struct imagepacknode * rv;

  rv = malloc(sizeof(struct imagepacknode));
  assert(rv != NULL);
  if(rv != NULL)
    {
      rv->image = NULL;
      rv->left  = NULL;
      rv->right = NULL;
      rv->x = x;
      rv->y = y;
      rv->w = width;
      rv->h = height;
    }
  else
    fprintf(stderr, "%s(%u, %u, %u, %u): Failed to allocate memory: %s\n", __FUNCTION__, (unsigned int) x, (unsigned int) y, (unsigned int) width, (unsigned int) height, strerror(errno));

  return rv;
}


struct imagepacknode * image_pack_free(struct imagepacknode * node)
{
  assert(node != NULL);

  if(node->left != NULL)
    node->left = image_pack_free(node->left);

  if(node->right != NULL)
    node->right = image_pack_free(node->right);

  return NULL;
}


struct imagepacknode * image_pack(struct imagepacknode * node, struct image * image)
{
  struct imagepacknode * rv;

  assert(node != NULL);
  assert(image != NULL);

  assert((node->left == NULL && node->right == NULL) || 
         (node->left != NULL && node->right != NULL)    );
  
  rv = NULL;
  if(node->left != NULL)
    { /* Not a leaf. */
      rv = image_pack(node->left, image);
      if(rv == NULL)
        rv = image_pack(node->right, image);
    }
  else
    { /* Leaf. */
      if(node->image == NULL)
        { /* Not taken. */
          if(node->w == image->width && node->h == image->height)
            { /* Perfect fit. */
              rv = node;
              rv->image = image;
            }
          else if(node->w >= image->width && node->h >= image->height)
            { /* Image fits in here. Split this node in two. */
              if(node->w - image->width > node->h - image->height)
                { /* More space horizontally. */
                  node->left  = image_pack_new(node->x, node->y, image->width, node->h);
                  node->right = image_pack_new(node->x + image->width, node->y, node->w - image->width, node->h);
                }
              else
                { /* More space vertically. */
                  node->left  = image_pack_new(node->x, node->y, node->w, image->height);
                  node->right = image_pack_new(node->x, node->y + image->height, node->w, node->h - image->height);
                }

              rv = image_pack(node->left, image);
            }
        }
    }

  return rv;
}
