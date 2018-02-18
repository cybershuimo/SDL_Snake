/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>
#include "02_headtest.h"

//Starts up SDL and creates window
bool init();

//Frees media and shuts down SDL
void close();

//Screen dimension constants
//const int SCREEN_WIDTH = 640;
//const int SCREEN_HEIGHT = 480;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
    
//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//The image we will load and show on the screen
SDL_Surface* gHelloWorld = NULL;


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
        gWindow = SDL_CreateWindow( "Test Window", SDL_WINDOWPOS_UNDEFINED, 
            SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN );
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

//Frees media and shuts down SDL
void close()
{
    //Deallocate surface
    SDL_FreeSurface( gHelloWorld );
    gHelloWorld = NULL;

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
		gHelloWorld = loadMedia( gHelloWorld );
		if ( gHelloWorld == NULL )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{
			//Apply the image
			SDL_BlitSurface( gHelloWorld, NULL, gScreenSurface, NULL );
			
			//Update the surface
            //SDL_UpdateWindowSurface( gWindow );
            if ( SDL_UpdateWindowSurface( gWindow ) != 0 )
			{
				printf( "Unable to update window %s! SDL Error: %s\n", "gWindow", SDL_GetError() );
			}
			else
			{
				printf("Update window successfully.\n");
			}

			//Wait two seconds
			SDL_Delay( 5000 );
		}
	}

	//Frees media and shuts down SDL
	close();

	return 0;
}