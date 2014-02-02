#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

struct layer {
	int width;
	int height;
	int visible;
	int **data;
};

struct tilesheet {
	char *image;
	int img_width;
	int img_height;
	int tile_width;
	int tile_height;
};

struct map {
	int width;
	int height;
	int tile_width;
	int tile_height;
	int num_layers;
	struct tilesheet *sheet;
	struct layer **layers;
};

#endif // MAP_H_INCLUDED
