#ifndef FILE_LOADERS_H_INCLUDED
#define FILE_LOADERS_H_INCLUDED

#include "map.h"


SDL_Surface *load_image(char *filename);
SDL_Rect *create_tileset(struct tilesheet *sheet);

#endif // FILE_LOADERS_H_INCLUDED