#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "map.h"
#include <stdio.h>


SDL_Surface *load_image(char *filename) {
	SDL_Surface* loadedImage = NULL;
    SDL_Surface* optimizedImage = NULL;

    loadedImage = IMG_Load(filename);

    if(loadedImage != NULL) {
        optimizedImage = SDL_DisplayFormat(loadedImage);

        SDL_FreeSurface(loadedImage);

        if(optimizedImage != NULL) {
            SDL_SetColorKey(
            	optimizedImage,
            	SDL_SRCCOLORKEY,
            	SDL_MapRGB(optimizedImage->format, 0, 0xFF, 0xFF)
            	);
        }
    }

    return optimizedImage;
}


SDL_Rect *create_tileset(struct tilesheet *sheet) {
	SDL_Rect *clip;
	int tiles = (sheet->img_width / sheet->tile_width) * (sheet->img_height / sheet->tile_height);
	int i, width = 0, height = 0;

	clip = (SDL_Rect *) calloc(tiles, sizeof(clip));

	for (i = 0; i < tiles; i++) {
		printf("Width %d, Height %d\n", width, height);

		clip[ 0 ].x = width;;
	    clip[ 0 ].y = height;
	    clip[ 0 ].w = sheet->tile_width;
	    clip[ 0 ].h = sheet->tile_height;

	    if (width >= sheet->img_width - sheet->tile_width) {
			width = 0;
			height += sheet->tile_height;
		}

		else {
			width += sheet->tile_width;
		}
	}

	return clip;
}