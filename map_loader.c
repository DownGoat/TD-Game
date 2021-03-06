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
 *
 * @param map_file The path of the map file to load.
 *
 * @return Return a heap allocated buffer with the data in the map file.
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
 * @param width The width of the map in tiles NOT pixels.
 * @param height The height of the map in tiles NOT pixels.
 * @param data The JSON map data.
 *
 * @return Returns array of arrays populated with the map data. Returns NULL
 *		   if something goes wrong.
 */
int **allocate_layer_array(int width, int height, json_t *data) {
	int height_count, json_idx = 0;
	int **array = (int **) calloc(height, sizeof(array));

	if (array == NULL) {
		fprintf(stderr, "Error allocating array in allocate_layer_array.\n");
		goto error;
	}

	for (height_count = 0; height_count < height; height_count++) {
		int width_count;
		int *tmp = (int *) calloc(width, sizeof(array));

		if (tmp == NULL) {
			array = NULL;
			fprintf(stderr, "Error allocating array in allocate_layer_array.\n");
			goto error;
		}
	
		for (width_count = 0; width_count < width; width_count++) {
			int a = json_integer_value(json_array_get(data, json_idx));
			tmp[width_count] = a;
			json_idx++;
		}

		array[height_count] = tmp;
	}

error:
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
 * Unpacks layers from the JSON data and stores the layers in a array.
 *
 * @param layers JSON data of the layers.
 * @param num_layers Pointer to int which will be set to the number of layers.
 *
 * @return Returns NULL if something goes wrong, returns array of layers if successful. 
 * 
 */
struct layer **load_layers(json_t *layers, int *num_layers) {
	int i, array_size;
	struct layer **the_array;


	array_size = json_array_size(layers);
	*num_layers = array_size;

	the_array = (struct layer **) calloc(array_size, sizeof(the_array));

	for(i = 0; i < json_array_size(layers); i++) {
		json_t *layer, *height, *width, *visible, *data;

		struct layer *current = (struct layer *) calloc(1, sizeof(current));

		layer = json_array_get(layers, i);

		height = json_object_get(layer, "height");
		if (!height) {
			fprintf(stderr, "Error getting height value in load_layers.\n");
			goto error;
		}

		current->height = (int) json_integer_value(height);

		width = json_object_get(layer, "width");
		if (!width) {
			fprintf(stderr, "Error getting width value in load_layers.\n");
			goto error;
		}

		current->width = (int) json_integer_value(width);

		visible = json_object_get(layer, "visible");
		if (!visible) {
			fprintf(stderr, "Error getting visible value in load_layers.\n");
			goto error;
		}

		current->visible = (int) json_is_true(visible);

		data = json_object_get(layer, "data");
		if (!json_is_array(data)) {
			fprintf(stderr, "Error getting height data in load_layers.\n");
			goto error;
		}

		current->data = allocate_layer_array(current->width, current->height, data);
		if (current->data == NULL) {
			fprintf(stderr, "Error allocate_layer_array in load_layers.\n");
			goto error;
		}
		the_array[i] = current;
	}

	return the_array;

error:
	// Need to loop over the stuff in the array and free it.
	for (; i > 0; i--) {
		free(the_array[i]);
	}

	free(the_array);

	return NULL;
}

/**
 *	This function prints out the data in the map structure. For debugging
 */
void print_map_data(struct map *map) {
	int i;

	printf("Width: %d\nHeight: %d\nTile width: %d\nTile height: %d\n",
		map->width, map->height, map->tile_width, map->tile_height
		);

	for (i = 0; i < map->num_layers; i++) {
		struct layer *layer  = map->layers[i];

		printf("Width %d\nHeight %d\nVisible %d\n",
			layer->width,
			layer->height,
			layer->visible
			);

		print_layer(layer);
	}
}

struct tilesheet *load_tileset(json_t *tilesets) {
	struct tilesheet *sheet;
	json_t *tmp, *img_height, *img_width, *tile_width, *tile_height, *filename;

	sheet = (struct tilesheet *) calloc(1, sizeof(sheet));

	tmp = json_array_get(tilesets, 0);

	
	img_height = json_object_get(tmp, "imageheight");
	if (!img_height) {
		fprintf(stderr, "Error getting imageheight value in load_tileset.\n");
		goto error;
	}

	sheet->img_height = (int) json_integer_value(img_height);

	
	img_width = json_object_get(tmp, "imagewidth");
	if (!img_width) {
		fprintf(stderr, "Error getting imagewidth value in load_tileset.\n");
		goto error;
	}

	sheet->img_width = (int) json_integer_value(img_width);


	tile_width = json_object_get(tmp, "tilewidth");
	if (!tile_width) {
		fprintf(stderr, "Error getting tile_width value in load_tileset.\n");
		goto error;
	}

	sheet->tile_width = (int) json_integer_value(tile_width);


	tile_height = json_object_get(tmp, "tileheight");
	if (!tile_height) {
		fprintf(stderr, "Error getting tile_height value in load_tileset.\n");
		goto error;
	}

	sheet->tile_height = (int) json_integer_value(tile_height);

	
	filename = json_object_get(tmp, "image");
	if (!filename) {
		fprintf(stderr, "Error getting filename value in load_tileset.\n");
		goto error;
	}

	sheet->image = (char *) json_string_value(filename);

	return sheet;

error:
	free(sheet);
	return NULL;
}


/**
 * This function opens a map file and loads the map data, and populates the map structure 
 * with the data from the file. 
 *
 * @param map_file Path to the map file.
 *
 * @return Returns NULL if something goes wrong, returns the map structure if successful.
 */
struct map *load_map(char *map_file) {
	char *map_json;
	//char *map_file = "test_map.json";
	struct map *map;

	json_t *root, *layers, *tilesets;
	json_error_t error;

	// Alloc the map structs
	map = (struct map *) calloc(1, sizeof(map));

	map_json = load_map_file(map_file);
	root = json_loads(map_json, 0, &error);

	map->width = (int) json_integer_value(json_object_get(root, "width"));
	map->height = (int) json_integer_value(json_object_get(root, "height"));
	map->tile_width = (int) json_integer_value(json_object_get(root, "tilewidth"));
	map->tile_height = (int) json_integer_value(json_object_get(root, "tileheight"));

	if (!root) {
		fprintf(stderr, "%s error: on line %d: %s\n",
			map_file, error.line, error.text);
		map = NULL;
		goto out;
	}

	if (!json_is_object(root)) {
	   	fprintf(stderr, "Is the loaded file really JSON?\n");
	   	map = NULL;
	   	goto out;
	}

	layers = json_object_get(root, "layers");
	if (!json_is_array(layers)) {
		fprintf(stderr, "%s: error: on line %d: %s\n",
			map_file, error.line, error.text);
		map = NULL;
		goto out;
	}

	map->layers = load_layers(layers, &map->num_layers);

	if (map->layers == NULL) {
		fprintf(stderr, "map is null\n");
		map = NULL;
		goto out;
	}

	tilesets = json_object_get(root, "tilesets");
	if (!json_is_array(tilesets)) {
		fprintf(stderr, "%s: error: on line %d: %s\n",
			map_file, error.line, error.text);
		map = NULL;
		goto out;
	}

	map->sheet = load_tileset(tilesets);
	if (map->sheet == NULL) {
		fprintf(stderr, "load_tileset failed\n");
		map = NULL;
		goto out;
	}
	

out:
	json_decref(root);
	free(map_json);

	return map; 
}