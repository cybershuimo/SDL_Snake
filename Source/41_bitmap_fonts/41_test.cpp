//41_bitmap_fonts, using a texture as a font type and handle keyboard inputs

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

//Our bitmap font
class LBitmapFont
{
    public:
        //The default constructor
        LBitmapFont();

        //Generates the font
        bool buildFont( LTexture *bitmap );

        //Shows the text
        void renderText( int x, int y, std::string text );

    private:
        //The font texture
        LTexture* mBitmap;

        //The individual characters in the surface
        SDL_Rect mChars[ 256 ];

        //Spacing Variables
        int mNewLine, mSpace;
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
LTexture gFontTexture;
//LTexture gInputTextTexture;


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


// LBitmapFont class member functions
LBitmapFont::LBitmapFont()
{
	//Initialize variables
    mBitmap = NULL;
    mNewLine = 0;
    mSpace = 0;
}

bool LBitmapFont::buildFont( LTexture *bitmap )
{
	bool success = true;
    
    //Lock pixels for access
    if( !bitmap->lockTexture() )
    {
        printf( "Unable to lock bitmap font texture!\n" );
        success = false;
    }
    else
    {
        //Set the background color
        Uint32 bgColor = bitmap->getPixel32( 0, 0 );

        //Set the cell dimensions; 16x16
        int cellW = bitmap->getWidth() / 16;
        int cellH = bitmap->getHeight() / 16;

        //New line variables
        int top = cellH;
        int baseA = cellH;

        //The current character we're setting
        int currentChar = 0;

        //Go through the cell rows
        for( int rows = 0; rows < 16; ++rows )
        {
            //Go through the cell columns
            for( int cols = 0; cols < 16; ++cols )
            {
                //Set the character offset
                mChars[ currentChar ].x = cellW * cols;
                mChars[ currentChar ].y = cellH * rows;

                //Set the dimensions of the character
                mChars[ currentChar ].w = cellW;
                mChars[ currentChar ].h = cellH;

                //Find the edge of the glyph sprite
                //Find Left Side
                //Go through pixel columns
                for( int pCol = 0; pCol < cellW; ++pCol )
                {
                    //Go through pixel rows
                    for( int pRow = 0; pRow < cellH; ++pRow )
                    {
                        //Get the pixel offsets
                        int pX = ( cellW * cols ) + pCol;
                        int pY = ( cellH * rows ) + pRow;

                        //If a non colorkey pixel is found
                        if( bitmap->getPixel32( pX, pY ) != bgColor )
                        {
                            //Set the x offset
                            mChars[ currentChar ].x = pX;

                            //Break the loops
                            pCol = cellW;
                            pRow = cellH;
                        }
                    }
                }

                //Find Right Side
                //Go through pixel columns
                for( int pCol = cellW; pCol > 0; --pCol )
                {
                    //Go through pixel rows
                    for( int pRow = 0; pRow < cellH; ++pRow )
                    {
                        //Get the pixel offsets
                        int pX = ( cellW * cols ) + pCol;
                        int pY = ( cellH * rows ) + pRow;

                        //If a non colorkey pixel is found
                        if( bitmap->getPixel32( pX, pY ) != bgColor )
                        {
                            //Set the x offset
                            mChars[ currentChar ].w = pX - mChars[ currentChar ].x + 1;

                            //Break the loops
                            pCol = 0;
                            pRow = cellH;
                        }
                    }
                }

                //Find Top
                //Go through pixel columns
                for( int pRow = 0; pRow < cellH; ++pRow )
                {
                    //Go through pixel rows
                    for( int pCol = 0; pCol < cellW; ++pCol )
                    {
                        //Get the pixel offsets
                        int pX = ( cellW * cols ) + pCol;
                        int pY = ( cellH * rows ) + pRow;

                        //If a non colorkey pixel is found
                        if( bitmap->getPixel32( pX, pY ) != bgColor )
                        {
							//If new top is found
                            if( pRow < top )
                            {
                                top = pRow;
                            }

                            //Break the loops
                            pCol = cellW;
                            pRow = cellH;
                        }
                    }
                }

                //Find Bottom of A
                if( currentChar == 'A' )
                {
                    //Go through pixel rows
                    for( int pRow = cellH - 1; pRow >= 0; --pRow )
                    {
                        //Go through pixel columns
                        for( int pCol = 0; pCol < cellW; ++pCol )
                        {
                            //Get the pixel offsets
                            int pX = ( cellW * cols ) + pCol;
                            int pY = ( cellH * rows ) + pRow;

                            //If a non colorkey pixel is found
                            if( bitmap->getPixel32( pX, pY ) != bgColor )
                            {
                                //Bottom of a is found
                                baseA = pRow;

                                //Break the loops
                                pCol = cellW;
                                pRow = -1;
                            }
                        }
                    }
                }

                //Go to next character
                ++currentChar;
            }
        }

        //Calculate space; set it to 0.5x character width
        mSpace = cellW / 2;

        //Calculate new line; 1x character height
        mNewLine = baseA - top;

        //Lop off excess top pixels
        for( int i = 0; i < 256; ++i )
        {
            mChars[ i ].y += top;
            //mChars[ i ].h = baseA - top;
            mChars[ i ].h -= top;	//why?
        }

        bitmap->unlockTexture();
        mBitmap = bitmap;
    }

    return success;
}

void LBitmapFont::renderText( int x, int y, std::string text )
{
	//If the font has been built
    if( mBitmap != NULL )
    {
        //Temp offsets
        int curX = x, curY = y;

        //Go through the text
        for( unsigned int i = 0; i < text.length(); ++i )
        {
            //If the current character is a space
            if( text[ i ] == ' ' )
            {
                //Move over
                curX += mSpace;
            }
            //If the current character is a newline
            else if( text[ i ] == '\n' )
            {
                //Move down
                curY += mNewLine;

                //Move back
                curX = x;
            }
            else
            {
                //Get the ASCII value of the character
                int ascii = (unsigned char)text[ i ];

                //Show the character
                mBitmap->render( curX, curY, &mChars[ ascii ] );

                //Move over the width of the character with one pixel of padding
                curX += mChars[ ascii ].w + 1;
            }
        }
    }
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
        gWindow = SDL_CreateWindow( "SDL Test_41", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
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

    //Load dot texture
    if( !gFontTexture.loadFromFile( "41_bitmap_fonts/lazyfont.png" ) )
    {
        printf( "Failed to load font texture!\n" );
        success = false;
    }

    return success;
}

void close()
{
    //Free loaded images
    gFontTexture.free();
    //gInputTextTexture.free();
    
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
            printf( "Failed to load media!\n" );
        }
        else
        {   
            //Main loop flag
            bool quit = false;

            //Event handler
            SDL_Event e;

            //Build customized font
            LBitmapFont gBitmapFont;
            gBitmapFont.buildFont( &gFontTexture );

            //Set text color as black
            SDL_Color textColor = { 0, 0, 0, 0xFF };

            //The current input text (default content)
            std::string inputText = "Input text: \n";

            //Enable text input, use SDL_StopTextInput() when finished
            SDL_StartTextInput();

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
                    //Special key input
                    else if( e.type == SDL_KEYDOWN )
                    {
                        //Handle backspace
                        if( e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0 )
                        {
                            //lop off character
                            inputText.pop_back();
                            //renderText = true;
                        }
                        //Handle copy
                        else if( e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
                        {
                            SDL_SetClipboardText( inputText.c_str() );
                        }
                        //Handle paste, how to free clipboard after using it?
                        else if( e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
                        {
                            inputText = SDL_GetClipboardText();
                            //renderText = true;
                        }
                    }
                    // else if( e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN )
                    // {
                    // 	inputText += '\n';
                    // 	printf("There is an Enter pressed.\n");
                    // }
                    //With text input enabled, key presses will also generate SDL_TextInputEvents which simplifies things like shift key and caps lock.
                    //Special text input event
                    else if( e.type == SDL_TEXTINPUT )
                    {
                        //Not copy or pasting
                        if( !( ( e.text.text[ 0 ] == 'c' || e.text.text[ 0 ] == 'C' ) && ( e.text.text[ 0 ] == 'v' || e.text.text[ 0 ] == 'V' ) && SDL_GetModState() & KMOD_CTRL ) )
                        {
                            //Append character
                            inputText += e.text.text;
                            //renderText = true;
                        }
                    }
                }
                
                //Clear screen
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( gRenderer );

                //gBitmapFont.renderText( 10, 10, inputText );
				gBitmapFont.renderText( 0, 0, "Bitmap Font:\nABDCEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789" );

                //Update screen
                SDL_RenderPresent( gRenderer );
            }

            //Disable text input
    		SDL_StopTextInput();
        }
    }
    
    //Free resources and close SDL
    close();

    return 0;
}