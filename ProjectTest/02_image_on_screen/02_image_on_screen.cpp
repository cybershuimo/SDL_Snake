/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>
#include "02_functions.h"


//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
    
//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//The image we will load and show on the screen
SDL_Surface* gHelloWorld = NULL;


int main( int argc, char* args[] )
{

	//Initialize SDL
	if ( !init( gWindow, gScreenSurface ) )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load image
		if ( !loadMedia( gHelloWorld ) )
		{
			printf( "Failed to load media!\n" );
		}
		else
        {
            //Apply the image
            SDL_BlitSurface( gHelloWorld, NULL, gScreenSurface, NULL );
        }

        //Update the surface
        SDL_UpdateWindowSurface( gWindow );
        //Wait two seconds
        SDL_Delay( 2000 );
	}

	//...
	close( gWindow, gHelloWorld );

	return 0;
}