//10_move_test
// Animated sprite, and Vertical Sync
// Render multiple images on the screen, some of which has transparent backgrounds
// Use keyboard input to move sprite on background texture

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
        bool loadFromFile( std::string path, int flag = SDL_FALSE,
         int transparentR = 0, int transparentG = 0, int transparentB = 0 );

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

//Walking animation, clip rendering
const int WALKING_ANIMATION_FRAMES = 4;
SDL_Rect gSpriteClips[ WALKING_ANIMATION_FRAMES ];

//Scene textures (two image to render)
LTexture gFooTexture;
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
bool LTexture::loadFromFile( std::string path, int flag, int transparentR, int transparentG, int transparentB )
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
        // int flag set SDL_TRUE to enable color key, SDL_FALSE to disable color key
        SDL_SetColorKey( loadedSurface, flag, SDL_MapRGB( loadedSurface->format, transparentR, 
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
        gWindow = SDL_CreateWindow( "SDL Tutorial_10", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Create renderer for window
            //we want to use Vertical Sync. VSync allows the rendering to update at the same time as when your monitor updates during vertical refresh. (Not run too fast.)
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
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
                int imgFlags = IMG_INIT_JPG;
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

    //Load background texture
    if( !gBackgroundTexture.loadFromFile( "10_move_test/background.png" ) )
    {
        printf( "Failed to load background texture!\n" );
        success = false;
    }

    //Load sprite sheet texture
    if( !gFooTexture.loadFromFile( "14_animated_sprites_and_vsync/foo.png", SDL_TRUE, 0, 255, 255 ) )
    {
        printf( "Failed to load walking animation texture!\n" );
        success = false;
    }
    else
    {
        
        //Set sprite clips
        gSpriteClips[ 0 ].x =   0;
        gSpriteClips[ 0 ].y =   0;
        gSpriteClips[ 0 ].w =  64;
        gSpriteClips[ 0 ].h = 205;

        gSpriteClips[ 1 ].x =  64;
        gSpriteClips[ 1 ].y =   0;
        gSpriteClips[ 1 ].w =  64;
        gSpriteClips[ 1 ].h = 205;
        
        gSpriteClips[ 2 ].x = 128;
        gSpriteClips[ 2 ].y =   0;
        gSpriteClips[ 2 ].w =  64;
        gSpriteClips[ 2 ].h = 205;

        gSpriteClips[ 3 ].x = 196;
        gSpriteClips[ 3 ].y =   0;
        gSpriteClips[ 3 ].w =  64;
        gSpriteClips[ 3 ].h = 205;
    }

    return success;
}

void close()
{
    //Free loaded image
    gFooTexture.free();
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

            //Track current animation frame
            int frame = 0;

            //Movement handler; starting position
            int x = 240;
            int y = 100;

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
                    //On keypress change rgb values
                    else if( e.type == SDL_KEYDOWN )
                    {
                        switch( e.key.keysym.sym )  //.sym = SDL_Keycode
                        {
                            //Move up
                            case SDLK_w:
                            y -= 20;
                            break;
                            
                            //Move down
                            case SDLK_s:
                            y += 20;
                            break;
                            
                            //Move left
                            case SDLK_a:
                            x -= 20;
                            break;
                            
                            //Move right
                            case SDLK_d:
                            x += 20;
                            break;
                        }
                    }
                }

				//Clear screen
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );
                
                //Render background texture to screen
                gBackgroundTexture.render( 0, 0 );

                //Determine which frame to render, change sprite clip every 6 frames
                SDL_Rect* currentClip = &gSpriteClips[ frame / 6 ];

                //Check if movement out of board
                if ( x < 0 )
                {
                    x = 0;
                }
                else if ( x + currentClip->w > gBackgroundTexture.getWidth() )
                {
                    x = gBackgroundTexture.getWidth() - currentClip->w;
                }
                else if ( y < 0 )
                {
                    y = 0;
                }
                else if ( y + currentClip->h > gBackgroundTexture.getHeight() )
                {
                    y = gBackgroundTexture.getHeight() - currentClip->h;
                }

                //Render sprite texture to screen
                gFooTexture.render( x, y, currentClip );

                //Update screen
                SDL_RenderPresent( gRenderer );

                //Go to next frame
                ++frame;

                //Cycle animation
                if( frame / 6 >= WALKING_ANIMATION_FRAMES )
                {
                    frame = 0;
                }
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}