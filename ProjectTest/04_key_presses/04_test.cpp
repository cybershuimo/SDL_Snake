// 04_test.cpp, makes different images appear depending on which key you press.

//Using SDL, standard IO, and strings
#include <SDL.h>
#include <stdio.h>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Key press surfaces constants
//int KEY_PRESS_SURFACE_DEFAULT = 0; int KEY_PRESS_SURFACE_UP = 1; etc.
enum KeyPressSurfaces
{
    KEY_PRESS_SURFACE_DEFAULT,
    KEY_PRESS_SURFACE_UP,
    KEY_PRESS_SURFACE_DOWN,
    KEY_PRESS_SURFACE_LEFT,
    KEY_PRESS_SURFACE_RIGHT,
    KEY_PRESS_SURFACE_TOTAL
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Loads individual image function
SDL_Surface* loadSurface( std::string path );

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
    
//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//The images that correspond to a keypress, array length KEY_PRESS_SURFACE_TOTAL + 1
SDL_Surface* gKeyPressSurfaces[ KEY_PRESS_SURFACE_TOTAL ];

//Current displayed image
SDL_Surface* gCurrentSurface = NULL;



//Starts up SDL and creates window
bool init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Create window
        gWindow = SDL_CreateWindow( "Customized Window", SDL_WINDOWPOS_UNDEFINED, 
        	SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Get window surface
            gScreenSurface = SDL_GetWindowSurface( gWindow );
        }
    }

    return success;
}

bool loadMedia()
{
    //Loading success flag
    bool success = true;

    //Load default surface
    gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ] = loadSurface( "04_key_presses/press.bmp" );
    if( gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ] == NULL )
    {
        printf( "Failed to load default image!\n" );
        success = false;
    }

    //Load up surface
    gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ] = loadSurface( "04_key_presses/up.bmp" );
    if( gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ] == NULL )
    {
        printf( "Failed to load up image!\n" );
        success = false;
    }

    //Load down surface
    gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ] = loadSurface( "04_key_presses/down.bmp" );
    if( gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ] == NULL )
    {
        printf( "Failed to load down image!\n" );
        success = false;
    }

    //Load left surface
    gKeyPressSurfaces[ KEY_PRESS_SURFACE_LEFT ] = loadSurface( "04_key_presses/left.bmp" );
    if( gKeyPressSurfaces[ KEY_PRESS_SURFACE_LEFT ] == NULL )
    {
        printf( "Failed to load left image!\n" );
        success = false;
    }

    //Load right surface
    gKeyPressSurfaces[ KEY_PRESS_SURFACE_RIGHT ] = loadSurface( "04_key_presses/right.bmp" );
    if( gKeyPressSurfaces[ KEY_PRESS_SURFACE_RIGHT ] == NULL )
    {
        printf( "Failed to load right image!\n" );
        success = false;
    }

    return success;
}

// This function allocate a pointer to an SDL_Surface, remember to deallocate it after calling function
SDL_Surface* loadSurface( std::string path )
{
    //Load image at specified path
    SDL_Surface* loadedSurface = SDL_LoadBMP( path.c_str() );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
    }

    return loadedSurface;
}

//Frees media and shuts down SDL
void close()
{
    //Deallocate surface
    for ( int i = 0; i < KEY_PRESS_SURFACE_TOTAL; i++ )
    {
    	SDL_FreeSurface( gKeyPressSurfaces[ i ] );
    	gKeyPressSurfaces[ i ] = NULL;
    }

    //Destroy window
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}

int main( int argc, char* args[] )
{

	//Initialize SDL
	if ( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load image
		if ( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
        {
	        //Main loop flag
	        bool quit = false;

	        //Event handler
	        SDL_Event e;

	        //Set default current surface
	        gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ];

	        //While application is running
	        while( !quit )
	        {
	            //Handle events on queue
	            while( SDL_PollEvent( &e ) != 0 )
	            {
	                //User requests quit
	                if( e.type == SDL_QUIT )
	                {
	                    quit = true;
	                }
	                else if( e.type == SDL_KEYDOWN)
	                {
	                	//Select surfaces based on key press
		                switch( e.key.keysym.sym )
		                {
		                    case SDLK_UP:
		                    gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ];
		                    break;

		                    case SDLK_DOWN:
		                    gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ];
		                    break;

		                    case SDLK_LEFT:
		                    gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_LEFT ];
		                    break;

		                    case SDLK_RIGHT:
		                    gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_RIGHT ];
		                    break;

		                    default:
		                    gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ];
		                    break;
		                }
		            }

	                //Apply the image
	                SDL_BlitSurface( gCurrentSurface, NULL, gScreenSurface, NULL );
	            
	                //Update the surface
	                SDL_UpdateWindowSurface( gWindow );
	        	}
        	}    
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}