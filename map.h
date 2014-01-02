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
	int width;
	int height;
	int tilewidth;
	int tileheight;
};

struct map {
	int width;
	int height;
	int tilewidth;
	int tileheight;
	int num_layers;
	struct tilesheet *sheet;
	struct layer **layers;
};

#endif // MAP_H_INCLUDED
