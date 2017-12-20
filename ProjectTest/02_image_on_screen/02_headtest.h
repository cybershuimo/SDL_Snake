//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>

//Starts up SDL and creates window
/*bool init( SDL_Window* gWindow, SDL_Surface* gScreenSurface )
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
}*/

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

//Frees media and shuts down SDL
void close( SDL_Window* gWindow, SDL_Surface* gHelloWorld )
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
