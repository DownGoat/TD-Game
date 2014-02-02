#include "SDL/SDL.h"
#include "map_loader.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "file_loaders.h"

// gcc *.c -l /usr/local/lib/libjansson.so -o TD_Game  -Wall 

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int SCREEN_BPP = 32;

//The surfaces
SDL_Surface *dots = NULL;
SDL_Surface *screen = NULL;

//The event structure
SDL_Event event;

//The portions of the sprite map to be blitted
SDL_Rect clip[ 4 ];


int init() {
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return 0;
    }

    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return 0;
    }

    //Set the window caption
    SDL_WM_SetCaption( "TD Game", NULL );

    //If everything initialized fine
    return 1;
}

void main_loop() {
	int quit = 0;

	init();
	//Update the screen
    if( SDL_Flip( screen ) == -1 )
    {
        return;
    }

    //While the user hasn't quit
    while( quit == 0 )
    {
        //While there's events to handle
        while( SDL_PollEvent( &event ) )
        {
            //If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = 1;
            }
        }
    }
}

int main( int argc, char* args[] ) {
	char *map_file = "test_map.json";
	struct map *map;
    
    map = load_map(map_file);
	print_map_data(map);

	create_tileset(map->sheet);

	main_loop();

    return 0;
}