#include "SDL/SDL.h"
#include "map_loader.h"
#include <pthread.h>


int main( int argc, char* args[] ) {
	char *map_file = "test_map.json";
    
	print_map_data(load_map(map_file));

    return 0;
}