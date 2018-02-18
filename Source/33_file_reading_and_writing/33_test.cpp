//33_file_reading_and_writing, save and load data from a binary file

//Using SDL, SDL_image, standard IO, and strings
#include <iostream>
// #include <windows.h>    // for Sleep()

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>

//check memory leak
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>

#include <vld.h>


//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int TOTAL_DATA = 10;

//The dot that will move around on the screen
class Dot
{
    public:
        //The dimensions of the dot
        static const int DOT_WIDTH = 20;
        static const int DOT_HEIGHT = 20;

        //Maximum axis velocity of the dot, speed 1 to move the camera
        static const int DOT_VEL = 10;

        //Initializes the variables
        Dot();

        //Takes key presses and adjusts the dot's velocity
        void handleEvent( SDL_Event& e );   // pass a reference of e as parameter

        //Moves the dot and checks collision
        void move();

        //Shows the dot on the screen relative to the camera
        void render();

        // //Position accessors
        // int getPosX();
        // int getPosY();

    private:
        //The X and Y offsets of the dot (left-top corner)
        int mPosX, mPosY;

        //The velocity of the dot
        int mVelX, mVelY;
};


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
        //If SDL_ttf is not defined, ignore this piece of code.
        bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
        #endif

        //Deallocates texture
        void free();

        //Set color modification of texture
        void setColor( Uint8 red, Uint8 green, Uint8 blue );

        //Set blending
        void setBlendMode( SDL_BlendMode blending );

        //Set alpha modulation
        void setAlpha( Uint8 alpha );

        //Renders texture at given point, and whether it is a clip
        void render( int x, int y, SDL_Rect* clip = NULL, 
            double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

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

//Globally used font
TTF_Font *gFont = NULL;

// //Texture to render
// LTexture gDotTexture;
LTexture gPromptTextTexture;
LTexture gDataTextures[ TOTAL_DATA ];

//Data points to save and load. For the demo length of 10
Sint32 gData[ TOTAL_DATA ];


// Class Dot, a subclass of LTexture?
Dot::Dot()
{
    //Initialize the offsets
    mPosX = ( SCREEN_WIDTH - DOT_WIDTH ) / 2;
    mPosY = ( SCREEN_HEIGHT - DOT_HEIGHT ) / 2;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
}

void Dot::handleEvent( SDL_Event& e )   // why not handleEvent( SDL_Event e )?
{
    //If a key was pressed but not a repeat. Only care when the key was first pressed
    if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY -= DOT_VEL; break;  // move left
            case SDLK_DOWN: mVelY += DOT_VEL; break;
            case SDLK_LEFT: mVelX -= DOT_VEL; break;    // move up
            case SDLK_RIGHT: mVelX += DOT_VEL; break;
        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY += DOT_VEL; break;  // stop moving left
            case SDLK_DOWN: mVelY -= DOT_VEL; break;
            case SDLK_LEFT: mVelX += DOT_VEL; break;    // stop moving up
            case SDLK_RIGHT: mVelX -= DOT_VEL; break;
        }
    }
}

void Dot::move()
{
    //Move the dot left or right
    mPosX += mVelX;

    //If the camera view went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > SCREEN_WIDTH ) )
    {
        //Move back
        mPosX -= mVelX;
    }

    //Move the dot up or down
    mPosY += mVelY;

    //If the camera view went too far to the up or down
    if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > SCREEN_HEIGHT ) )
    {
        //Move back
        mPosY -= mVelY;
    }

}

// void Dot::render()
// {
//     //Show the dot relative to the camera
//     gDotTexture.render( mPosX, mPosY );
// }

// //Position accessors
// int Dot::getPosX()
// {
//     return mPosX;
// }

// int Dot::getPosY()
// {
//     return mPosY;
// }


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
        gWindow = SDL_CreateWindow( "SDL Test_33_file_reading_and_writing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
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

                //Initialize SDL_ttf
                if( TTF_Init() == -1 )
                {
                    printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
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

    //Open the font
    gFont = TTF_OpenFont( "16_true_type_fonts/lazy.ttf", 28 );
    if( gFont == NULL )
    {
        printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }
    else
    {
        //Load prompt text
        SDL_Color textColor = { 0, 0, 0, 0xFF };
        if( !gPromptTextTexture.loadFromRenderedText( "Enter Data: ", textColor ) )
        {
            printf( "Failed to load background texture!\n" );
            success = false;
        }
    }

    //Open file for reading in binary
    SDL_RWops* file = SDL_RWFromFile( "33_file_reading_and_writing/nums.bin", "r+b" );

    //File does not exist
    if( file == NULL )
    {
        printf( "Warning: Unable to open file! SDL Error: %s\n", SDL_GetError() );
        
        //Create file for writing
        file = SDL_RWFromFile( "33_file_reading_and_writing/nums.bin", "w+b" );
        if( file != NULL )
        {
            printf( "New file created!\n" );

            //Initialize data
            for( int i = 0; i < TOTAL_DATA; ++i )
            {
                gData[ i ] = 0;    
                SDL_RWwrite( file, &gData[ i ], sizeof(Sint32), 1 );
            }
            
            //Close file handler
            SDL_RWclose( file );
        }
        else
        {
            printf( "Error: Unable to create file! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
    }
    //File exists
    else
    {
        //Load data
        printf( "Reading file...!\n" );
        for( int i = 0; i < TOTAL_DATA; ++i )
        {
            SDL_RWread( file, &gData[ i ], sizeof(Sint32), 1 );
        }

        //Close file handler
        SDL_RWclose( file );
    }


    //Text rendering color
    SDL_Color textColor = { 0, 0, 0, 0xFF };
    SDL_Color highlightColor = { 0xFF, 0, 0, 0xFF };

    //Initialize data textures, 0th data highlighted, others default color
    gDataTextures[ 0 ].loadFromRenderedText( std::to_string( ( long long )gData[ 0 ] ), highlightColor );
    for( int i = 1; i < TOTAL_DATA; ++i )
    {
        gDataTextures[ i ].loadFromRenderedText( std::to_string( ( long long )gData[ i ] ), textColor );
    }

    return success;
}

void close()
{
    //Open save file for writing
    SDL_RWops* file = SDL_RWFromFile( "33_file_reading_and_writing/nums.bin", "w+b" );
    if( file != NULL )
    {
        //Save data
        for( int i = 0; i < TOTAL_DATA; ++i )
        {
            SDL_RWwrite( file, &gData[ i ], sizeof(Sint32), 1 );
        }

        //Close file handler
        SDL_RWclose( file );
    }
    else
    {
        printf( "Error: Unable to save file! %s\n", SDL_GetError() );
    }

    // //Free loaded image
    // gDotTexture.free();
    gPromptTextTexture.free();
    for (int i = 0; i < TOTAL_DATA; ++i)
    {
    	gDataTextures[ i ].free();
    }
    

    //Free global font
    TTF_CloseFont( gFont );
    gFont = NULL;
    
    //Destroy window
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gRenderer = NULL;
    gWindow = NULL;

    //Quit SDL subsystems
    //Clean up all dynamically loaded library handles, freeing memory
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    printf( "Unload resource successfully. Close the window as you like.\n" );
    std::cin.get(); 

    // // Delay 5000ms
    // Sleep( 5000 );
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

            //Text rendering color
            SDL_Color textColor = { 0, 0, 0, 0xFF };
            SDL_Color highlightColor = { 0xFF, 0, 0, 0xFF };

            //Current input point
            int currentData = 0;

            //While application is running
            while( !quit )
            {
                //The rerender text flag
                bool renderText = false;

                //Handle events on queue
                while( SDL_PollEvent( &e ) != 0 )
                {
                    //User requests quit
                    if( e.type == SDL_QUIT )
                    {
                        quit = true;
                    }
                    //Handle keyboard inputs, change num chosen, or change num value
                    else if( e.type == SDL_KEYDOWN )
                    {
                        switch( e.key.keysym.sym )
                        {
                            //Previous data entry
                            case SDLK_UP:
                            //Rerender previous entry input point
                            gDataTextures[ currentData ].loadFromRenderedText( std::to_string( ( long long )gData[ currentData ] ), textColor );
                            --currentData;
                            if( currentData < 0 )
                            {
                                currentData = TOTAL_DATA - 1;
                            }
                            //Rerender current entry input point
                            gDataTextures[ currentData ].loadFromRenderedText( std::to_string( ( long long )gData[ currentData ] ), highlightColor );
                            break;
                            
                            //Next data entry
                            case SDLK_DOWN:
                            //Rerender previous entry input point
                            gDataTextures[ currentData ].loadFromRenderedText( std::to_string( ( long long )gData[ currentData ] ), textColor );
                            ++currentData;
                            if( currentData == TOTAL_DATA )
                            {
                                currentData = 0;
                            }
                            //Rerender current entry input point
                            gDataTextures[ currentData ].loadFromRenderedText( std::to_string( ( long long )gData[ currentData ] ), highlightColor );
                            break;

                            //Decrement input point
                            case SDLK_LEFT:
                            --gData[ currentData ];
                            gDataTextures[ currentData ].loadFromRenderedText( std::to_string( ( long long )gData[ currentData ] ), highlightColor );
                            break;
                            
                            //Increment input point
                            case SDLK_RIGHT:
                            ++gData[ currentData ];
                            gDataTextures[ currentData ].loadFromRenderedText( std::to_string( ( long long )gData[ currentData ] ), highlightColor );
                            break;
                        }
                    }

                }
                

                //Clear screen
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( gRenderer );

                //Render text textures
                gPromptTextTexture.render( ( SCREEN_WIDTH - gPromptTextTexture.getWidth() ) / 2, 0 );
                for (int i = 0; i < TOTAL_DATA; ++i)
                {
                	gDataTextures[ i ].render( ( SCREEN_WIDTH - gDataTextures[ i ].getWidth() ) / 2, 
                		gPromptTextTexture.getHeight() + gDataTextures[ 0 ].getHeight() * i );
                }

                //Update screen
                SDL_RenderPresent( gRenderer );
            }
        }
    }

    //Disable text input
    SDL_StopTextInput();
    
    //Free resources and close SDL
    close();

    return 0;
}
