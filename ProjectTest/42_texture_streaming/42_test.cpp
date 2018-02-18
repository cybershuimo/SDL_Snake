//42_texture_streaming. Using texture stream we can render pixels from any source

//Using SDL, SDL_image, standard IO, and strings
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
//#include <SDL_ttf.h>
#include <stdio.h>
#include <string>

//check memory leak
#include <vld.h>


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
        
        #ifdef _SDL_TTF_H
        //Creates image from font string
        bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
        #endif

        //Creates blank texture -- NEW FUNCTIONALITY HERE
        bool createBlank( int width, int height );

        //Deallocates texture
        void free();

        //Set color modulation
        void setColor( Uint8 red, Uint8 green, Uint8 blue );

        //Set blending
        void setBlendMode( SDL_BlendMode blending );

        //Set alpha modulation
        void setAlpha( Uint8 alpha );
        
        //Renders texture at given point
        void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

        //Gets image dimensions
        int getWidth();
        int getHeight();

        //Pixel manipulators
        bool lockTexture();
        bool unlockTexture();
        void* getPixels();
        void copyPixels( void* pixels );	//copies in the pixel data we want to stream
        int getPitch();
        Uint32 getPixel32( unsigned int x, unsigned int y );

    private:
        //The actual hardware texture
        SDL_Texture* mTexture;
        void* mPixels;
        int mPitch;

        //Image dimensions
        int mWidth;
        int mHeight;
};


//A test animation stream
class DataStream
{
    public:
        //Initializes internals
        DataStream();

        //Loads initial data
        bool loadMedia();

        //Deallocator
        void free();

        //Gets current frame data
        void* getBuffer();

    private:
        //Internal data
        SDL_Surface* mImages[ 4 ];
        int mCurrentImage;
        int mDelayFrames;	//what for?
};


//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

// Global variables
//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Texture to render
LTexture gStreamingTexture;

//Data stream (source)
DataStream gDataStream;

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

    //The final texture
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );

    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
        success = false;
    }
    else
    {
        //Convert surface to display format
        SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat( loadedSurface, SDL_GetWindowPixelFormat( gWindow ), NULL );
        if( formattedSurface == NULL )
        {
            printf( "Unable to convert loaded surface to display format! SDL Error: %s\n", SDL_GetError() );
        }
        else
        {
            //Create blank streamable/lockable texture
            newTexture = SDL_CreateTexture( gRenderer, SDL_GetWindowPixelFormat( gWindow ), 
            	SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h );
            if( newTexture == NULL )
            {
                printf( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
            }
            else
            {
            	//Enable blending on texture
                SDL_SetTextureBlendMode( newTexture, SDL_BLENDMODE_BLEND );

                //Lock texture for manipulation
                SDL_LockTexture( newTexture, &formattedSurface->clip_rect, &mPixels, &mPitch );

                //Copy loaded/formatted surface pixels
                memcpy( mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h );

                //Get image dimensions
                mWidth = formattedSurface->w;
                mHeight = formattedSurface->h;

                //Get pixel data in editable format
                Uint32* pixels = (Uint32*)mPixels;
                int pixelCount = ( mPitch / 4 ) * mHeight;

                //Map colors                
                Uint32 colorKey = SDL_MapRGB( formattedSurface->format, 0, 0xFF, 0xFF );
                Uint32 transparent = SDL_MapRGBA( formattedSurface->format, 0xFF, 0xFF, 0xFF, 0x00 );

                //Color key pixels
                for( int i = 0; i < pixelCount; ++i )
                {
                    if( pixels[ i ] == colorKey )
                    {
                        pixels[ i ] = transparent;
                    }
                }

                //Unlock texture to update
                SDL_UnlockTexture( newTexture );
                mPixels = NULL;
            }

            //Get rid of old formatted surface
            SDL_FreeSurface( formattedSurface );
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }

    //Return success
    mTexture = newTexture;
    return mTexture != NULL;
}


#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
    //Get rid of preexisting texture
    free();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
    if( textSurface == NULL )
    {
        printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
        if( mTexture == NULL )
        {
            printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        }
        else
        {
            //Get image dimensions
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        //Get rid of old surface
        SDL_FreeSurface( textSurface );
    }
    
    //Return success
    return mTexture != NULL;
}
#endif

//Creates blank texture
bool LTexture::createBlank( int width, int height )
{
	//Create uninitialized texture
    mTexture = SDL_CreateTexture( gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height );
    if( mTexture == NULL )
    {
        printf( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
    }
    else
    {
        mWidth = width;
        mHeight = height;
    }

    return mTexture != NULL;
}

void LTexture::free()
{
    //Free texture if it exists
    if ( mTexture != NULL )
    {
        SDL_DestroyTexture( mTexture );
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

//Renders texture at given point
void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
    //Set destination rectangle
    SDL_Rect renderQuad = { x, y, mWidth, mHeight };

    //Set renderQuad width and height same as source rect
    if ( clip != NULL )
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    
    SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
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

bool LTexture::lockTexture()
{
	bool success = true;

	// if texture is already locked
	if ( mPixels != NULL )
	{
		printf( "Texture is already locked!\n" );
        success = false;
	}
	// if not, lock texture
	else
	{
		if ( SDL_LockTexture( mTexture, NULL, &mPixels, &mPitch ) != 0 )
		{
			printf( "Unable to lock texture! %s\n", SDL_GetError() );
            success = false;
		}
	}

	return success;
}

bool LTexture::unlockTexture()
{
	bool success = true;

	// if texture is already unlocked
	if ( mPixels == NULL )
	{
		printf( "Texture is already unlocked!\n" );
        success = false;
	}
	// if not, unlock texture
	else
	{
		SDL_UnlockTexture( mTexture );
		mPixels = NULL;
		mPitch = 0;	//why?
	}

	return success;
}

void* LTexture::getPixels()
{
    return mPixels;
}

void LTexture::copyPixels( void* pixels )
{
	// Check if texture is locked
    if( mPixels != NULL )
    {
        //Copy to locked pixels
        memcpy( mPixels, pixels, mPitch * mHeight );
    }
}

int LTexture::getPitch()
{
    return mPitch;
}

// calculate pixel offsets like Y Offset * Pitch + X Offset
Uint32 LTexture::getPixel32( unsigned int x, unsigned int y )
{
	//Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32*)mPixels;

    //Get the pixel requested
    return pixels[ ( y * ( mPitch / 4 ) ) + x ];
}


// DataStream class member functions
DataStream::DataStream()
{
	for (int i = 0; i < 4; ++i)
	{
		mImages[ i ] = NULL;
	}
	
    int mCurrentImage = 0;
    int mDelayFrames = 0;	
}

bool DataStream::loadMedia()
{
	//Loading success flag
	bool success = true;

	for( int i = 0; i < 4; ++i )
	{
		char path[ 64 ] = "";
		sprintf_s( path, "42_texture_streaming/foo_walk_%d.png", i );

		SDL_Surface* loadedSurface = IMG_Load( path );
		if( loadedSurface == NULL )
		{
			printf( "Unable to load %s! SDL_image error: %s\n", path, IMG_GetError() );
			success = false;
		}
		else
		{
			mImages[ i ] = SDL_ConvertSurfaceFormat( loadedSurface, SDL_PIXELFORMAT_RGBA8888, NULL );
		}

		SDL_FreeSurface( loadedSurface );
	}
	
	return success;
}

void DataStream::free()
{
	for (int i = 0; i < 4; ++i)
	{
		if ( mImages[ i ] != NULL )
		{
			SDL_FreeSurface( mImages[ i ] );
			mImages[ i ] = NULL;
		}	
	}
}

//get pixel data of current image frame
void* DataStream::getBuffer()
{
	// Set current frame, play slower than current image count
    int currentFrame = mCurrentImage / 6;
    ++mCurrentImage;

    if ( mCurrentImage >= 24 )
	{
		mCurrentImage = 0;
	}

	return mImages[ currentFrame ]->pixels;
}


bool init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Create window
        gWindow = SDL_CreateWindow( "SDL Test_42", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Create vsynced renderer for window
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
                int imgFlags = IMG_INIT_PNG;
                if( !( IMG_Init( imgFlags ) & imgFlags ) )
                {
                    printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                    success = false;
                }

                // //Initialize SDL_ttf
                // if( TTF_Init() == -1 )
                // {
                //     printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
                //     success = false;
                // }
            }
        }
    }

    return success;
    //Filled gWindow, gRenderer
}


// Load resource and do pixel modification
bool loadMedia()
{
    //Loading success flag
    bool success = true;

    //Load blank texture
	if( !gStreamingTexture.createBlank( 64, 205 ) )
	{
		printf( "Failed to create streaming texture!\n" );
		success = false;
	}

    //Load dot texture
    if( !gDataStream.loadMedia() )
    {
        printf( "Failed to load stream source!\n" );
        success = false;
    }

    return success;
}

void close()
{
    //Free loaded images
    gStreamingTexture.free();
    gDataStream.free();

    //Destroy window
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gRenderer = NULL;
    gWindow = NULL;

    //Quit SDL subsystems
    //Clean up all dynamically loaded library handles, freeing memory
    //TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    printf( "Unload resource successfully. Close the window as you like.\n" );
    std::cin.get(); 
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
            printf( "Failed to load stream media!\n" );
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

                //Copy frame from buffer
                gStreamingTexture.lockTexture();
                gStreamingTexture.copyPixels( gDataStream.getBuffer() );
                gStreamingTexture.unlockTexture();

                //Render frame
                gStreamingTexture.render( ( SCREEN_WIDTH - gStreamingTexture.getWidth() ) / 2, ( SCREEN_HEIGHT - gStreamingTexture.getHeight() ) / 2 );

                //Update screen
                SDL_RenderPresent( gRenderer );
            }
        }
    }
    
    //Free resources and close SDL
    close();

    return 0;
}