//class prototypes，Snake class and Food class
//basic actions, snake head move

//Using SDL, SDL_image, standard IO, and strings
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
//#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
//#include <fstream>  //file reading and writing with SDL RWOps
//#include <vector>

//check memory leak
#include <vld.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Snake body length limit
const int BODY_LIMIT = 100;

//The snake body that follows snake head
class Tile
{
    public:
        //The dimensions of the snake body tile
        static const int TILE_WIDTH = 20;
        static const int TILE_HEIGHT = 20;

        //Initializes
        Tile( int x, int y );

        //Sets position
        void setPosition( int x, int y );

        //Shows the tile
        void render();

        //Get the collision box
        SDL_Rect getBox();

    private:
        //The attributes of the tile
        SDL_Rect mBox;
};


//The snake that will move around on the screen
class Snake
{
    public:
        //The dimensions of the snake
        static const int SNAKE_WIDTH = 20;
        static const int SNAKE_HEIGHT = 20;

        //Maximum axis velocity of the snake
        static const int SNAKE_VEL = 20;

        //Initializes the variables and allocates particles
        Snake();

        // //Deallocates particles
        // ~Snake();

        //Takes key presses and adjusts the snake's velocity
        void handleEvent( SDL_Event& e );

        //Moves the snake
        void move( float timeStep );

        //Shows the snake on the screen
        void render();

        //Gets length of snake body
        int getLength();

        //Get the collision box
        SDL_Rect getBox();

    private:
        //Collision box of the snake
        SDL_Rect mBox;

        //The velocity of the snake
        float mVelX, mVelY;

        //The body length of the snake
        int mBody;
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


//The application time based timer
class LTimer
{
    public:
        //Initializes variables
        LTimer();

        //The various clock actions
        void start();
        void stop();
        void pause();
        void unpause();

        //Gets the timer's time
        Uint32 getTicks();

        //Checks the status of the timer
        bool isStarted();
        bool isPaused();

    private:
        //The clock time when the timer started
        Uint32 mStartTicks;

        //The ticks stored when the timer was paused
        Uint32 mPausedTicks;

        //The timer status
        bool mPaused;
        bool mStarted;

        //Why not duration?
};
//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Store last position, move one by one
void moveBody( Tile* snakeBody[], int length, int posX, int posY );

//Box collision detector
bool checkCollision( SDL_Rect a, SDL_Rect b );

// Global variables
//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Texture to render
LTexture gSnakeTexture;
LTexture gBodyTexture;

//The snake that will be moving around on the screen
Snake snake;
//Snake body tiles
Tile* snakeBody[ BODY_LIMIT ];

// Class Tile, used for snake body tiles
//Initializes position and type
Tile::Tile( int x, int y )
{
    //Get the offsets
    setPosition( x, y );
    
    //Set the collision box
    mBox.w = TILE_WIDTH;
    mBox.h = TILE_HEIGHT;
}

void Tile::setPosition( int x, int y )
{
    //Set the offsets
    mBox.x = x;
    mBox.y = y;
}

//Show the tile
void Tile::render()
{
    gBodyTexture.render( mBox.x, mBox.y );
}

//Get the collision box
SDL_Rect Tile::getBox()
{
    return mBox;
}


// Class Snake
Snake::Snake()
{
    //Initialize the offsets
    mBox.x = 360;
    mBox.y = 200;
    
    //Set the collision box
    mBox.w = SNAKE_WIDTH;
    mBox.h = SNAKE_HEIGHT;

    //Initialize the velocity
    mVelX = -SNAKE_VEL;
    mVelY = 0;

    //Initialize the snake body length
    mBody = 5;
}

// //Deallocates particles
// Snake::~Snake()
// {
//     for( int i = 0; i < TOTAL_PARTICLES; ++i )
//     {
//         particles[ i ]->mTexture = NULL;
//         delete particles[ i ];
//     }
// }

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

void Snake::move( float timeStep )
{
    //Store last position
    moveBody( snakeBody, mBody, mBox.x, mBox.y );

    //Move the snake left or right
    mBox.x += mVelX * timeStep;

    //If the snake touches the boarder
    if ( mBox.x < 0 )
    {
        mBox.x = 0;
    }
    else if( mBox.x + SNAKE_WIDTH > SCREEN_WIDTH )
    {
        //Move back
        mBox.x = SCREEN_WIDTH - SNAKE_WIDTH;
    }

    //Move the snake up or down
    mBox.y += mVelY * timeStep;

    //If the snake touches the boarder
    if ( mBox.y < 0 )
    {
        mBox.y = 0;
    }
    else if( mBox.y + SNAKE_HEIGHT > SCREEN_HEIGHT )
    {
        //Move back
        mBox.y = SCREEN_HEIGHT - SNAKE_HEIGHT;
    }
}


void Snake::render()
{
    //Show the snake relative to the camera
    gSnakeTexture.render( mBox.x, mBox.y );
}

int Snake::getLength()
{
    return mBody;
}

SDL_Rect Snake::getBox()
{
    return mBox;
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

bool loadMedia()
{
    //Loading success flag
    bool success = true;

    //Load snake head texture
    if( !gSnakeTexture.loadFromFile( "_graphic/SnakeHead.png" ) )
    {
        printf( "Failed to load snake head texture!\n" );
        success = false;
    }
    else if( !gBodyTexture.loadFromFile( "_graphic/SnakeBody-1.png" ) )
    {
        printf( "Failed to load snake body texture!\n" );
        success = false;
    }

    return success;
}

void close()
{
    //Free loaded images
    gSnakeTexture.free();
    gBodyTexture.free();

    //Free global font
    // TTF_CloseFont( gFont );
    // gFont = NULL;
    
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

void moveBody( Tile* snakeBody[], int length, int posX, int posY )
{
    for (int i = length - 1; i > 0; --i)
    {
        //Store the followed tile position
        SDL_Rect lastPosition = snakeBody[ i - 1 ]->getBox();
        if ( checkCollision( snakeBody[ i - 1 ]->getBox(), snakeBody[ i ]->getBox() ) == false )
        {
            snakeBody[ i ]->setPosition( lastPosition.x, lastPosition.y );
        }
    }

    if ( checkCollision( snakeBody[ 0 ]->getBox(), snake.getBox() ) == false )
    {
        snakeBody[ 0 ]->setPosition( posX, posY );
    }
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

            int snakeBodyLength = snake.getLength();
            int posX = 360;   // set it as the offset X of snake head 
            int posY = 200;   // set it as the offset Y of snake head

            //Initialize snake body
            for (int i = 0; i < snakeBodyLength; ++i)
            {
                posX += 20;
                snakeBody[ i ] = new Tile( posX, posY );
            }

            //Keeps track of time between steps
            LTimer stepTimer;

            //The camera area
            //SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

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

                    //Handle input for the camera
                    snake.handleEvent( e );
                }

                //Calculate time step; why 1000, move too slowly?
                float timeStep = stepTimer.getTicks() / 100.f;

                //Move the snake
                snake.move( timeStep );
                
                //Restart step timer; too early?
                stepTimer.start();

                //Clear screen; black background color
                SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
                SDL_RenderClear( gRenderer );

                //Render objects
                snake.render();
                for (int i = 0; i < snakeBodyLength; ++i)
                {
                    snakeBody[ i ]->render();
                }

                //Update screen
                SDL_RenderPresent( gRenderer );
            }

            //Deallocate snake body tiles
            for( int i = 0; i < snakeBodyLength; ++i )
            {
                 if( snakeBody[ i ] != NULL )
                 {
                    delete snakeBody[ i ];
                    snakeBody[ i ] = NULL;
                 }
            }
        }

    }

    //Free resources and close SDL
    close();

    return 0;
}