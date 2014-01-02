#ifndef MAP_LOADER_H_INCLUDED
#define MAP_LOADER_H_INCLUDED

#include <jansson.h>

char *load_map_file(char *map_file);
void load_map();
struct layer **load_layers(json_t *layers, int *num_layers);
void print_layer(struct layer *current);

#endif // MAP_LOADER_H_INCLUDED
