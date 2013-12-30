#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

struct layer {
	int width;
	int height;
	int visible;
	int **data;
};

struct layers {
	struct layers_element *head;
	struct layers_element *tail;
	int size;
};

struct layers_element {
	struct layer *current;
	struct layers_element *next;
	struct layers_element *previous;
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
	struct tilesheet *sheet;
	struct layers *layers;
};

#endif // MAP_H_INCLUDED
