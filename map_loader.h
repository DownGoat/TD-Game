#ifndef MAP_LOADER_H_INCLUDED
#define MAP_LOADER_H_INCLUDED

#include <jansson.h>

char *load_map_file(char *map_file);
void load_map();
struct layers *load_layers(json_t *layers);

#endif // MAP_LOADER_H_INCLUDED
