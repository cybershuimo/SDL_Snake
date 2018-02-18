//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>


//Loads media
SDL_Surface* loadMedia( SDL_Surface* gHelloWorld )
{
    //Load splash image
    gHelloWorld = SDL_LoadBMP( "02_image_on_screen/FFXV.bmp" );
    if( gHelloWorld == NULL )
    {
        printf( "Unable to load image %s! SDL Error: %s\n", "02_image_on_screen/FFXV.bmp", SDL_GetError() );
    }
    else
    {
        printf("Load image successfully.\n");
    }

    return gHelloWorld;
}


