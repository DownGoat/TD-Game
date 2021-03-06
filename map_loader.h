#ifndef MAP_LOADER_H_INCLUDED
#define MAP_LOADER_H_INCLUDED

#include <jansson.h>

char *load_map_file(char *map_file);
struct map *load_map(char *map_file);
struct layer **load_layers(json_t *layers, int *num_layers);
void print_layer(struct layer *current);
void print_map_data(struct map *map);
struct tilesheet *load_tileset(json_t *tilesets);

#endif // MAP_LOADER_H_INCLUDED
