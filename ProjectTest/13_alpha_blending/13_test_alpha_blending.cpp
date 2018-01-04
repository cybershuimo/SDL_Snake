// 13_alpha_blending, alpha modulation to control the transparency of a texture

// Color modulation, alter the color of rendered textures
// int SDL_SetTextureColorMod();

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
        bool loadFromFile( std::string path );

        //Deallocates texture
        void free();

        //Renders texture at given point, and whether it is a clip
        void render( int x, int y, SDL_Rect* clip = NULL );

        //Set color modification of texture
        void setColor( Uint8 red, Uint8 green, Uint8 blue );

        //Set blending
        void setBlendMode( SDL_BlendMode blending );

        //Set alpha modulation
        void setAlpha( Uint8 alpha );

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
LTexture gBackgroundTexture;
LTexture gFrontTexture;


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
bool LTexture::loadFromFile( std::string path )
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
        //SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0, 0 ) );
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


//Set color modulation
void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	SDL_SetTextureColorMod( mTexture, red, green, blue );
	//srcC = srcC * (color / 255)
	//return -1 if color modulation is not supported.
}

//Set alpha modulation
void LTexture::setAlpha( Uint8 alpha )
{
	SDL_SetTextureAlphaMod( mTexture, alpha );
	//srcA = srcA * (alpha / 255)
	//return -1 if alpha modulation is not supported.
}

//Set blending, e.g.alpha blending
void LTexture::setBlendMode( SDL_BlendMode blending )
{
	SDL_SetTextureBlendMode( mTexture, blending );
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
        gWindow = SDL_CreateWindow( "SDL Test_13", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
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

    //Load front alpha texture
    if( !gFrontTexture.loadFromFile( "13_alpha_blending/fadeout.png" ) )
    {
        printf( "Failed to load front texture!\n" );
        success = false;
    }
    else
    {
    	//Set standard alpha blending
    	gFrontTexture.setBlendMode( SDL_BLENDMODE_BLEND );
    }

    //Load background texture
    if( !gBackgroundTexture.loadFromFile( "13_alpha_blending/fadein.png" ) )
    {
        printf( "Failed to load background texture!\n" );
        success = false;
    }

    return success;
}

void close()
{
    //Free loaded image
    gBackgroundTexture.free();
    gFrontTexture.free();

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

            //Modulation components
            Uint8 alpha = 255;

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
                        //Increase alpha on w
                        //.sym = SDL_Keycode
                        if ( e.key.keysym.sym == SDLK_w)
                        {
                        	//Cap if over 255
                        	if ( alpha + 64 > 255)
                        	{
                        		alpha = 255;
                        	}
                        	//Increment otherwise
                        	else
                        	{
                        		alpha += 64;
                        	}
                        }
                        //Decrease alpha on s
                        else if ( e.key.keysym.sym == SDLK_s)
                        {
                        	//Cap if below 0
                        	if ( alpha - 64 < 0)
                        	{
                        		alpha = 0;
                        	}
                        	//Decrement otherwise
                        	else
                        	{
                        		alpha -= 64;
                        	}
                        }
                    }
				}

				//Clear screen
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

                //Render textures to screen
                gBackgroundTexture.render( 0, 0 );

                gFrontTexture.setAlpha( alpha );
                gFrontTexture.render( 0, 0 );

                //Update screen
                SDL_RenderPresent( gRenderer );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}