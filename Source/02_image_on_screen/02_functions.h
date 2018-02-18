//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>

//Starts up SDL and creates window
bool init( SDL_Window* gWindow, SDL_Surface* gScreenSurface )
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

//Loads media
bool loadMedia( SDL_Surface* srcSurface )
{
    //Loading success flag
    bool success = true;

    //Load splash image
    //The new surface should be freed with SDL_FreeSurface().
    srcSurface = SDL_LoadBMP( "02_image_on_screen/FFXV.bmp" );

    if( srcSurface == NULL )
    {
        printf( "Unable to load image %s! SDL Error: %s\n", "02_image_on_screen/FFXV.bmp", SDL_GetError() );
        success = false;
    }
    else
    {
        printf( "Load image successfully.\n" );        
    }

    return success;
}

//Frees media and shuts down SDL
void close( SDL_Window* gWindow, SDL_Surface* gSurface )
{
    //Deallocate surface
    SDL_FreeSurface( gSurface );
    gSurface = NULL;

    //Destroy window
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}
