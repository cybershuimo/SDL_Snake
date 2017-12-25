// 04_test.cpp, makes different images appear depending on which key you press.

//Using SDL, standard IO, and strings
#include <SDL.h>
#include <stdio.h>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 960;

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
    gCurrentSurface = loadSurface( "05_optimized_surface_loading_and_soft_stretching/stretch.bmp" );
    if( gCurrentSurface == NULL )
    {
        printf( "Failed to load scaled image!\n" );
        success = false;
    }

    return success;
}

// This function allocate a pointer to an SDL_Surface, deallocate it after calling the function
SDL_Surface* loadSurface( std::string path )
{
    //Load image at specified path
    SDL_Surface* loadedSurface = SDL_LoadBMP( path.c_str() );
    SDL_Surface* optimizedSurface = NULL;

    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
    }
    else
    {
        //Convert surface to screen format
        optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, NULL );
        if( optimizedSurface == NULL )
        {
            printf( "Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }

    return optimizedSurface;
}

//Frees media and shuts down SDL
void close()
{
    //Deallocate surface
    SDL_FreeSurface( gCurrentSurface );
    gCurrentSurface = NULL;

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

	                //Apply the image
	                //SDL_BlitSurface( gCurrentSurface, NULL, gScreenSurface, NULL );

                    //Apply the image stretched
                    SDL_Rect stretchRect;
                    stretchRect.x = 480;
                    stretchRect.y = 360;
                    stretchRect.w = SCREEN_WIDTH;
                    stretchRect.h = SCREEN_HEIGHT;
                    SDL_BlitScaled( gCurrentSurface, NULL, gScreenSurface, &stretchRect );
	                //SDL_BlitSurface( gCurrentSurface, NULL, gScreenSurface, &stretchRect );

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