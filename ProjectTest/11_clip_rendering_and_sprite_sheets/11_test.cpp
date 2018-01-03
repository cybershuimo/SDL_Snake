//11_clip_rendering_and_sprite_sheet
// Render only part of a texture

//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Texture wrapper class
class LTexture
{
    public:
        //Initializes variables
        LTexture();

        //Deallocates memory
        ~LTexture();

        //Loads image at specified path
        bool loadFromFile( std::string path, int transparentR,  int transparentG, int transparentB );

        //Deallocates texture
        void free();

        //Renders texture at given point, and whether it is a clip
        void render( int x, int y, SDL_Rect* clip = NULL );

        //Gets image dimensions
        int getWidth();
        int getHeight();

    private:
        //The actual hardware texture
        SDL_Texture* mTexture;

        //Image dimensions
        int mWidth;
        int mHeight;
};


//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures (two image to render)
//Scene sprites
SDL_Rect gSpriteClips[ 4 ];
LTexture gSpriteSheetTexture;
LTexture gBackgroundTexture;


// Now to build the class methods
// Initializes
LTexture::LTexture()
{
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

// Deallocates texture
LTexture::~LTexture()
{
    //Free loaded image
    free();
}

// Load image from specific path
bool LTexture::loadFromFile( std::string path, int transparentR,  int transparentG, int transparentB )
{
    //Initialization flag
    bool success = true;
    //Get rid of preexisting texture
    free();

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
        success = false;
    }
    else
    {
        //Color key image, use SDL_SetColorKey() to set the color key (transparent pixel) in a surface
        SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, transparentR, 
            transparentG, transparentB ) );
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
        if( mTexture == NULL )
        {
            printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
            success = false;
        }
        else
        {
            //Fill width, height of the image
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;

        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }

    return success;
}


void LTexture::free()
{
    SDL_DestroyTexture( mTexture );
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

//Renders texture at given point
void LTexture::render( int x, int y, SDL_Rect* clip )
{
    //Set destination rectangle
    SDL_Rect renderQuad = { x, y, mWidth, mHeight };

    //Set renderQuad width and height same as source rect
    if ( clip != NULL )
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    
    SDL_RenderCopy( gRenderer, mTexture, clip, &renderQuad );
    //SDL_RenderCopy( renderer, texture, &srcrect, &dstrect );
}

//Get image dimensions
int LTexture::getWidth()
{
    return mWidth;
}

int LTexture::getHeight()
{
    return mHeight;
}



bool init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Create window
        gWindow = SDL_CreateWindow( "SDL Tutorial_11", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Create renderer for window
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
            if( gRenderer == NULL )
            {
                printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
            else
            {
                //Initialize renderer color, white?
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if( !( IMG_Init( imgFlags ) & imgFlags ) )
                {
                    printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                    success = false;
                }
            }
        }
    }

    return success;
    //Filled gWindow, gRenderer
}

bool loadMedia()
{
    //Loading success flag
    bool success = true;

    //Load JPG surface
    if( !gSpriteSheetTexture.loadFromFile( "11_clip_rendering_and_sprite_sheets/dots.png", 0, 0xFF, 0xFF ) )
    {
        printf( "Failed to load sprite sheet texture!\n" );
        success = false;
    }
    else if ( !gBackgroundTexture.loadFromFile( "02_image_on_screen/FFXV.jpg", 0, 0, 0 ) )
    {
        printf( "Failed to load backgroud texture!\n" );
        success = false;
    }
    else
    {
        //Set top left sprite
        gSpriteClips[ 0 ].x =   0;
        gSpriteClips[ 0 ].y =   0;
        gSpriteClips[ 0 ].w = 100;
        gSpriteClips[ 0 ].h = 100;

        //Set top right sprite
        gSpriteClips[ 1 ].x = 100;
        gSpriteClips[ 1 ].y =   0;
        gSpriteClips[ 1 ].w = 100;
        gSpriteClips[ 1 ].h = 100;
        
        //Set bottom left sprite
        gSpriteClips[ 2 ].x =   0;
        gSpriteClips[ 2 ].y = 100;
        gSpriteClips[ 2 ].w = 100;
        gSpriteClips[ 2 ].h = 100;

        //Set bottom right sprite
        gSpriteClips[ 3 ].x = 100;
        gSpriteClips[ 3 ].y = 100;
        gSpriteClips[ 3 ].w = 100;
        gSpriteClips[ 3 ].h = 100;
    }

    return success;
}

void close()
{
    //Free loaded image
    gSpriteSheetTexture.free();
    gBackgroundTexture.free();

    //Destroy window
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gRenderer = NULL;
    gWindow = NULL;

    //Quit SDL subsystems
    //Clean up all dynamically loaded library handles, freeing memory
    IMG_Quit();
    SDL_Quit();
}


int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
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
				}

				//Clear screen
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );
                
                //Render textures to screen
                gBackgroundTexture.render( 0, 0, NULL );
                gSpriteSheetTexture.render( 0, 0, &gSpriteClips[ 0 ] );
                gSpriteSheetTexture.render( SCREEN_WIDTH - gSpriteClips[ 1 ].w, 0, &gSpriteClips[ 1 ] );
                gSpriteSheetTexture.render( 0, SCREEN_HEIGHT - gSpriteClips[ 2 ].h, &gSpriteClips[ 2 ] );
                gSpriteSheetTexture.render( SCREEN_WIDTH - gSpriteClips[ 3 ].w, SCREEN_HEIGHT - gSpriteClips[ 3 ].h, &gSpriteClips[ 3 ] );

                //Update screen
                SDL_RenderPresent( gRenderer );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}