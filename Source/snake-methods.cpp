//Class methods and function declaration file

#include "snake.h"

//Using SDL, SDL_image, standard IO, and strings
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <cstdlib>  //rand() needed
#include <string>
#include <iomanip> // setprecision

//check memory leak
#include <vld.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Global variables
//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Globally used font
TTF_Font *gFont = NULL;

//The sound effects that will be used
//0 = FoodEaten; 1 = GameOver; 2 = PlayAgain
enum SoundEffects
{
    FOOD_EATEN,
    GAME_OVER,
    PLAY_AGAIN,
    SE_TOTAL
};

Mix_Chunk* gSoundEffects[ SE_TOTAL ];

//Textures to render
LTexture gTileTexture[ TYPE_TOTAL ];
LTexture gTextTexture;
LTexture gButtonOnTexture;
LTexture gButtonOffTexture;

LTexture gUITexture[ 2 ];

int SnakeBody::snakeBodyNumber = 0;


// Class Tile, used for snake body tiles
//Initializes position and type
Tile::Tile( int x, int y )
{
    //Get the offsets
    setPosition( x, y );
}

//Render tile
void Tile::render()
{
    gTileTexture[ mTileType ].render( mBox.x, mBox.y );
}

void Tile::setPosition( int x, int y )
{
    //Set the offsets
    mBox.x = x;
    mBox.y = y;
}

void Tile::setSize( int w, int h )
{
    //Set the width and height
    mBox.w = w;
    mBox.h = h;
}

void Tile::setTileType( unsigned int type )
{
    //Set the tile type
    mTileType = static_cast<TileType>( type );
}

//Get the collision box
SDL_Rect Tile::getBox()
{
    return mBox;
}

// Class SnakeBody
//Initializes
SnakeBody::SnakeBody( int x, int y ) : Tile( x, y )
{
    setTileType( snakeBodyNumber % TYPE_SNAKEHEAD );
    ++snakeBodyNumber;
    // printf( "New snakeBody part created. Location %i, %i\n", x, y );
}

//Snake body moves; following snake head one by one
void SnakeBody::move( int &posX, int &posY, SDL_Rect newRect, SDL_Rect headRect, bool &gameOverFlag )
{
    //Store last position
    int lastX = getBox().x;
    int lastY = getBox().y;

    setPosition( posX, posY );
    //printf( "posX is %i, posY is %i\n", posX, posY );

    // if collided with new rect, stand back
    if ( checkCollision( getBox(), newRect ) )
    {
        setPosition( lastX, lastY );
    }

    // if collided with snake head rect, set game over flag
    if ( checkCollision( getBox(), headRect ) )
    {
        gameOverFlag = true;
    }

    // pass last position to the following tile
    posX = lastX;
    posY = lastY;
}


// Class Food methods
//Initializes
Food::Food( int x, int y ) : Tile( x, y )
{
    //Initialize the tile type
    setTileType( TYPE_FOOD_0 );
    // printf( "New snakeBody part created. Location %i, %i\n", x, y );
}

//Appears at random position; TODO other better method to randomize position
//Play sound effect when food eaten
void Food::generate()
{
    int randX = rand() % 32;
    int randY = rand() % 24;
    setPosition( randX * 20, randY * 20 );
}

//Check if touched by snake head;
bool Food::eaten( SDL_Rect snakeHead )
{
    if ( checkCollision( getBox(), snakeHead ) )
    {
        return true;
    }

    return false;
}


// Class Snake methods
Snake::Snake( int x, int y ) : Tile( x, y )
{
    //Initialize the tile type
    setTileType( TYPE_SNAKEHEAD );

    //Initialize the velocity
    mVelX = -SNAKE_VEL;
    mVelY = 0;

    //Initialize the snake body length
    mBody = 5;
}

void Snake::handleEvent( SDL_Event& e )   // why not handleEvent( SDL_Event e )?
{
    //If a key was pressed but not a repeat. Only care when the key was first pressed
    if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP:  // move up
                mVelX = 0;
                mVelY = -SNAKE_VEL; 
                break;
            case SDLK_DOWN: // move down
                mVelX = 0;
                mVelY = SNAKE_VEL; 
                break;
            case SDLK_LEFT:     // move left
                mVelX = -SNAKE_VEL;
                mVelY = 0; 
                break;
            case SDLK_RIGHT:    // move right
                mVelX = SNAKE_VEL;
                mVelY =  0; 
                break;
        }
    }
    // //If a key was released
    // else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    // {
    //     //Adjust the velocity
    //     switch( e.key.keysym.sym )
    //     {
    //         case SDLK_UP: mVelY += SNAKE_VEL; break;  // stop moving left
    //         case SDLK_DOWN: mVelY -= SNAKE_VEL; break;
    //         case SDLK_LEFT: mVelX += SNAKE_VEL; break;    // stop moving up
    //         case SDLK_RIGHT: mVelX -= SNAKE_VEL; break;
    //     }
    // }
}

//Store last position to posX, posY, and move snake collision box
void Snake::move( int &posX, int &posY, bool &gameOverFlag )
{
    //Store last position; use reference &x to change arguments
    posX = mBox.x;
    posY = mBox.y;
    //printf( "posX stored %i, posY stored %i\n", posX, posY );
    
    //Move the snake left or right
    mBox.x += mVelX * TILE_WIDTH; 

    //If the snake touches the boarder
    if ( mBox.x < 0 )
    {
        mBox.x = 0;
        gameOverFlag = true;
    }
    else if( mBox.x + TILE_WIDTH > SCREEN_WIDTH )
    {
        //Move back
        mBox.x = SCREEN_WIDTH - TILE_WIDTH;
        gameOverFlag = true;
    }

    //Move the snake up or down
    mBox.y += mVelY * TILE_HEIGHT;

    //If the snake touches the boarder
    if ( mBox.y < 0 )
    {
        mBox.y = 0;
        gameOverFlag = true;
    }
    else if( mBox.y + TILE_HEIGHT > SCREEN_HEIGHT )
    {
        //Move back
        mBox.y = SCREEN_HEIGHT - TILE_HEIGHT;
        gameOverFlag = true;
    }
    
}

//Add one body tile after eating food
void Snake::addLength()
{
    ++mBody;
}

int Snake::getLength()
{
    return mBody;
}

void Snake::restart()
{
    //restart to initial condition(?)
    //Snake( 360, 200 ); //why not working?

    setPosition( 360, 200 );
    
    //Initialize the velocity
    mVelX = -SNAKE_VEL;
    mVelY = 0;

    //Initialize the snake body length
    mBody = 5;
}


// Class Button methods
//Initializes
Button::Button( int x, int y ) : Tile( x, y )
{
    setPosition( ( SCREEN_WIDTH - gButtonOffTexture.getWidth() ) / 2, ( SCREEN_HEIGHT - gButtonOffTexture.getHeight() ) * 2 / 3 );
    setSize( gButtonOffTexture.getWidth(), gButtonOffTexture.getHeight() );
    mButtonType = BUTTON_OFF;
}

//Handles mouse event
void Button::handleEvent( SDL_Event& e, bool &flag )
{
    //If mouse event happened
    if( e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP )
    {
        //Get mouse position
        int x, y;
        SDL_GetMouseState( &x, &y );
        //Check if mouse is in button
        bool inside = true;

        //Mouse is left of the button
        if( x < getBox().x )
        {
            inside = false;
        }
        //Mouse is right of the button
        else if( x > getBox().x + getBox().w )
        {
            inside = false;
        }
        //Mouse above the button
        else if( y < getBox().y )
        {
            inside = false;
        }
        //Mouse below the button
        else if( y > getBox().y + getBox().h )
        {
            inside = false;
        }

        //Mouse is inside button
        if( inside && e.type == SDL_MOUSEBUTTONDOWN)
        {
            // mouse button is pressed and heldon
            setButtonType( BUTTON_ON );
        }
        else if ( inside && e.type == SDL_MOUSEBUTTONUP )
        {
            // mouse button is released
            setButtonType( BUTTON_OFF );
            changeFlag( flag );
        }
    }
}

//Change external flag
void Button::changeFlag( bool &flag )
{
    flag = !flag;
}

//Set type of button
void Button::setButtonType( Button::ButtonType type )
{
    mButtonType = type;
}

void Button::render()
{
    switch( mButtonType )
    {
        case BUTTON_OFF:
        gButtonOffTexture.render( getBox().x, getBox().y ); //Temperary Texture
        break;

        case BUTTON_ON:
        gButtonOnTexture.render( getBox().x, getBox().y );  //Temperary Texture
        break;
    }
}


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


// Initializes
LTimer::LTimer()
{
    //Initialize the variables
    mStartTicks = 0;
    mPausedTicks = 0;

    mPaused = false;
    mStarted = false;
}

void LTimer::start()
{
    //If not started, start the timer
    if ( !mStarted )
    {
        mStarted = true;
    }
    //Get the current clock time
    mStartTicks = SDL_GetTicks();

    //Clear stored time
    mPausedTicks = 0;
}

void LTimer::stop()
{
    if ( mStarted )
    {
        mStarted = false;
    }
    mPausedTicks += SDL_GetTicks() - mStartTicks;

    //Clear tick variables
    mStarted = false;
    mPaused = false;

}

void LTimer::pause()
{
    if ( mStarted && !mPaused )
    {
        //Pause the timer
        mPaused = true;

        //Calculate the paused ticks
        mPausedTicks += SDL_GetTicks() - mStartTicks;

        //Clear start tick
        mStartTicks = 0;
    }
}   

void LTimer::unpause()
{
    if ( mPaused )
    {
        //Unpause the timer
        mPaused = false;

        // Start the tick
        mStartTicks = SDL_GetTicks();
    }    
}

Uint32 LTimer::getTicks()
{
    //The actual timer time, if not started yet
    Uint32 time = 0;

    // if started already, and paused at present
    if ( mStarted && mPaused )
    {
        time = mPausedTicks;
    }
    // if started but runs still
    else if ( mStarted && !mPaused )
    {
        time = SDL_GetTicks() - mStartTicks + mPausedTicks;
    }
    // if stopped, show time stored
    else if ( !mStarted && mPausedTicks )
    {
        time = mPausedTicks;
    }
    
    return time;
}

bool LTimer::isStarted()
{
    return mStarted;
}

bool LTimer::isPaused()
{
    return mStarted && mPaused;
}

// UI class methods
// Initializes
UI::UI()
{
    //Initialize the variables
    mStarted = false;
    numFoodEaten = 0;
}

void UI::updateFoodEaten()
{
    //Add num of food eaten by one
    ++numFoodEaten;

    Mix_PlayChannel( -1, gSoundEffects[ FOOD_EATEN ], 0 );    //Play SE once
}

void UI::render()
{
    // 0 = numFoodEaten; 1 = liveTime
    //Set text to be rendered
    using namespace std;

    liveTimeText.str( "" );
    liveTimeText << "LIVE TIME: " << fixed << setprecision(4) << liveTime.getTicks() / 1000.0;
    //liveTimeText << "LIVE TIME: " << fixed << setprecision(2) << liveTime.getTicks();
    numFoodEatenText.str( "" );
    numFoodEatenText << "FOOD EATEN: " << numFoodEaten;

    //Render text
    if( !gUITexture[ 0 ].loadFromRenderedText( liveTimeText.str().c_str(), textColor ) )
    {
        printf( "Failed to render UI liveTime texture!\n" );
    }
    else if( !gUITexture[ 1 ].loadFromRenderedText( numFoodEatenText.str().c_str(), textColor ) )
    {
        printf( "Failed to render UI numFoodEaten texture!\n" );
    }

    //Render text textures
    gUITexture[ 0 ].render( ( SCREEN_WIDTH / 2 - gUITexture[ 0 ].getWidth() ) / 2, 20 );
    gUITexture[ 1 ].render( ( SCREEN_WIDTH / 2 - gUITexture[ 0 ].getWidth() ) / 2 + SCREEN_WIDTH / 2, 20 );
}

//When game over, stop the timer
void UI::start()
{
    liveTime.start();
    mStarted = true;
}

//When game over, stop the timer; Play GAME_OVER SE once
void UI::paused()
{
    liveTime.stop();
    if ( mStarted )
    {
        Mix_Volume( -1, MIX_MAX_VOLUME/2 ); //Set all channel volumn to half
        Mix_PlayChannel( -1, gSoundEffects[ GAME_OVER ], 0 );    //Play GAME_OVER SE once
        Mix_Volume( -1, -1 );   //Unset channel volumns
    }
    mStarted = false;
}

void UI::restart()
{
    //liveTime.start();
    numFoodEaten = 0;

    Mix_PlayChannel( -1, gSoundEffects[ PLAY_AGAIN ], 0 );    //Play SE once
}

bool UI::isStarted()
{
    return mStarted;
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
        gWindow = SDL_CreateWindow( "SnakeTest-BasicActions", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
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

                //Initialize SDL_mixer
                if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
                {
                    printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
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
    using namespace std;
    std::stringstream filename;

    //Loading success flag
    bool success = true;

    //Load snake head, bocy, food texture
    for (int i = 0; i < 2; ++i)
    {
        filename.str( "" );
        filename << "../../Resource/SnakeBody_" << i << ".png";

        if( !gTileTexture[ i ].loadFromFile( filename.str().c_str() ) )
        {
            printf( "Failed to load snake body %i texture!\n", i );
            success = false;
        }
    }

    if( !gTileTexture[ TYPE_SNAKEHEAD ].loadFromFile( "../../Resource/SnakeHead.png" ) )
    {
        printf( "Failed to load snake head texture!\n" );
        success = false;
    }
    else if( !gTileTexture[ TYPE_FOOD_0 ].loadFromFile( "../../Resource/Food_0.png" ) )
    {
        printf( "Failed to load food texture!\n" );
        success = false;
    }

    //Load font texture
    //Open the font
    gFont = TTF_OpenFont( "../../Resource/game_over.ttf", 40 );

    if( gFont == NULL )
    {
        printf( "Failed to load game_over font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }
    else
    {
        // set the loaded font's outline to 5 pixel wide
        //TTF_SetFontOutline(gFont, 1);
        //Render text
        SDL_Color gameOverColor = { 200, 0, 0 };
        if( !gTextTexture.loadFromRenderedText( "GAME  OVER", gameOverColor ) )
        {
            printf( "Failed to render text texture!\n" );
            success = false;
        }

        SDL_Color snakeHeadColor = { 93, 188, 210 };
        if( !gButtonOffTexture.loadFromRenderedText( "PLAY  AGAIN", snakeHeadColor ) )
        {
            printf( "Failed to render PLAY AGAIN button texture!\n" );
            success = false;
        }
        
        SDL_Color snakeBodyColor = { 35, 177, 77 };
        if( !gButtonOnTexture.loadFromRenderedText( "PLAY  AGAIN", snakeBodyColor ) )
        {
            printf( "Failed to render PLAY AGAIN button texture!\n" );
            success = false;
        }

    }

    //Load sound effects
    for (int i = 0; i < SE_TOTAL; ++i)
    {
        filename.str( "" );
        filename << "../../Resource/SE_" << i << ".wav";

        gSoundEffects[ i ] = Mix_LoadWAV( filename.str().c_str() );
        if( gSoundEffects[ i ] == NULL )
        {
            printf( "Failed to load SoundEffects[ %i ]! SDL_mixer Error: %s\n", i, Mix_GetError() );
            success = false;
        }
    }

    return success;
}

void close()
{
    //Free loaded images
    for (int i = 0; i < TYPE_TOTAL; ++i)
    {
        gTileTexture[ i ].free();
    }
    
    gTextTexture.free();
    gButtonOnTexture.free();
    gButtonOffTexture.free();

    for (int i = 0; i < 2; ++i)
    {
        gUITexture[ i ].free();
    }

    //Free global font
    TTF_CloseFont( gFont );
    gFont = NULL;
    
    //Free sound effects
    for (int i = 0; i < SE_TOTAL; ++i)
    {
        if ( gSoundEffects[ i ] != NULL )
        {
            Mix_FreeChunk( gSoundEffects[ i ] );
            gSoundEffects[ i ] = NULL;
        }    
    }
    
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
}

bool checkCollision( SDL_Rect a, SDL_Rect b )
{
    bool collision = true;

    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    //Calculate the sides of rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    //do separating axis test
    if ( rightA <= leftB )
    {
        collision = false;
    }
    //Y axis value: the more bottom the bigger
    else if ( topA >= bottomB )
    {
        collision = false;
    }
    else if ( leftA >= rightB )
    {
        collision = false;
    }
    else if ( bottomA <= topB )
    {
        collision = false;
    }

    //If none of the sides from A are outside B
    return collision;
}