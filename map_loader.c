#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "map_loader.h"
#include "map.h"


extern int errno;

/**
 * This function loads a map file from disc and stores it in a buffer that is
 * returned.
 */
char *load_map_file(char *map_file) {
	char *buffer;
	long file_size;

	FILE *map_json = fopen(map_file, "r");
	if (map_json == NULL) {
		fprintf(stderr, "Failed to load map file: %s, errno: %d\n",
			map_file, errno);
		exit(0);
	}

	fseek(map_json, 0, SEEK_END);
	file_size = ftell(map_json);
	rewind(map_json);

	buffer = (char *) calloc(file_size, sizeof(buffer));

	fread(buffer, sizeof(char), file_size, map_json);

	fclose(map_json);

	return buffer;
}

/**
 * This function allocates the arrays that is used to store which tile goes
 * in which tile spot. The function also populates the array. A map has a given
 * width and height, which is the number of tiles not pixels. First a array
 * is allocated to be the same size of the width of the map. The second for
 * loop populates the width array with data, it reads in width number of
 * elements, since the json array is a 1d array it has to be read in a special
 * order to convert it to a 2d array.
 *
 */
int **allocate_layer_array(int width, int height, json_t *data) {
	int i, height_count, json_idx = 0;
	int **array = (int **) calloc(height, sizeof(int));

	for (height_count = 0; height_count < height; height_count++) {
		int width_count;
		int *tmp = (int *) calloc(width, sizeof(int));
	
		for (width_count = 0; width_count < width; width_count++) {
			int a = json_integer_value(json_array_get(data, json_idx));
			tmp[width_count] = a;
			json_idx++;
		}

		array[height_count] = tmp;
	}



	return array;
}

/**
 * This function prints out the layer data, only for debuging.
 */
void print_layer(struct layer *current) {
	int width_count, height_count;

	for (height_count = 0; height_count < current->height; height_count++) {
		for (width_count = 0; width_count < current->width; width_count++) {
			printf("%d", current->data[height_count][width_count]);
		}
		printf("\n");
	}
}

/**
 * Unpacks layers from the JSON data and stores the layers in a linked list
 * 
 */
struct layers *load_layers(json_t *layers) {
	int i;
	struct layers *layers_list = (struct layers *) calloc(1, sizeof(layers_list));

	for(i = 0; i < json_array_size(layers); i++) {
    	json_t *layer, *height, *width, *visible, *data;
    	struct layers_element *element = (struct layers_element *) calloc(
    		1, sizeof(element)
    		);

    	struct layer *current = (struct layer *) calloc(1, sizeof(current));
    	element->current = current;

    	layer = json_array_get(layers, i);

    	height = json_object_get(layer, "height");
    	current->height = (int) json_integer_value(height);

    	width = json_object_get(layer, "height");
    	current->width = (int) json_integer_value(width);

    	visible = json_object_get(layer, "visible");
    	current->visible = (int) json_integer_value(visible);

    	data = json_object_get(layer, "data");
    	if (json_is_array(data)) {
    		printf("have map data.\n");
    	}

    	current->data = allocate_layer_array(
    		current->width, current->height, data);

    	if (layers_list->head == NULL) {
    		layers_list->head = element;
    		layers_list->tail = element;
    		layers_list->size = 1;
    	}

    	else {
    		layers_list->tail->next = element;
    		element->previous = layers_list->tail;
    		layers_list->tail = element;
    		layers_list->size++;
    	}
    }

    return layers_list;
}

void print_map_data(struct map *map) {
	int i;
	struct layers_element *current = map->layers->head;

	printf("Width: %d\nHeight: %d\nTile width: %d\nTile height: %d\n",
		map->width, map->height, map->tilewidth, map->tileheight
		);

	while (current != NULL) {
		printf("Width %d\nHeight %d\nVisible %d\n", 
			current->current->width,
			current->current->height,
			current->current->visible
			);
		print_layer(current->current);
		current = current->next;
	}
}


void load_map() {
	char *map_json;
	char *map_file = "test_map.json";
	struct map *map;
	struct tilesheet *sheet;
	int i;

	json_t *root, *layers, *tilesets, *height, *width;
	json_t *tileheight, *tilewidth;
    json_error_t error;

    // Alloc the map structs
    map = (struct map *) calloc(1, sizeof(map));
    sheet = (struct tilesheet *) calloc(1, sizeof(sheet));

    map->sheet = sheet;

	map_json = load_map_file(map_file);
	root = json_loads(map_json, 0, &error);

	map->width = (int) json_integer_value(json_object_get(root, "width"));
    map->height = (int) json_integer_value(json_object_get(root, "height"));
    map->tilewidth = (int) json_integer_value(json_object_get(root, "tilewidth"));
    map->tileheight = (int) json_integer_value(json_object_get(root, "tileheight"));

	if (!root) {
		fprintf(stderr, "%s error: on line %d: %s\n",
			map_file, error.line, error.text);
		goto out;
	}

	if (!json_is_object(root)) {
       	fprintf(stderr, "Is the loaded file really JSON?\n");
       	goto out;
    }

    layers = json_object_get(root, "layers");
    if (!json_is_array(layers)) {
    	fprintf(stderr, "%s: error: on line %d: %s\n",
    		map_file, error.line, error.text);
    	goto out;
    }

    map->layers = load_layers(layers);
    if (map->layers == NULL) {
    	fprintf(stderr, "map is null\n");
    	goto out;
    }

    printf("Number of layers %d\n", map->layers->size);

    tilesets = json_object_get(root, "tilesets");
    if (!json_is_array(tilesets)) {
    	fprintf(stderr, "%s: error: on line %d: %s\n",
    		map_file, error.line, error.text);
    	goto out;
    }
    
    print_map_data(map);

out:
	free(map_json);
}