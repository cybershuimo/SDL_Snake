//To do per-pixel collision detection all we have to do is have each object have a set of box colliders 
//and check collision of one set of collision boxes against another

//Using SDL, SDL_image, standard IO, and strings
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
//#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//The dot that will move around on the screen
class Dot
{
    public:
        //The dimensions of the dot
        static const int DOT_WIDTH = 20;
        static const int DOT_HEIGHT = 20;

        //Maximum axis velocity of the dot
        static const int DOT_VEL = 1;

        //Initializes the variables
        Dot( int x, int y);

        //Takes key presses and adjusts the dot's velocity
        void handleEvent( SDL_Event& e );   // pass a reference of e as parameter

        //Moves the dot and checks collision
        void move( std::vector<SDL_Rect>& otherColliders );

        //Shows the dot on the screen
        void render();

        //Gets the collision boxes
        std::vector<SDL_Rect>& getColliders();

    private:
        //The X and Y offsets of the dot
        int mPosX, mPosY;

        //The velocity of the dot
        int mVelX, mVelY;

        //Dot's collision box
        //define a vector mColliders, of SDL_Rect;
        std::vector<SDL_Rect> mColliders;

        //Moves the collision boxes relative to the dot's offset
        void shiftColliders();
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

//Box collision detector
bool checkCollision( std::vector<SDL_Rect>& a, std::vector<SDL_Rect>& b );

// Global variables
//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

// //Globally used font
// TTF_Font *gFont = NULL;

//Texture to render
LTexture gDotTexture;


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


// Class Dot, a subclass of LTexture?
Dot::Dot( int x, int y )
{
    //Initialize the offsets
    mPosX = x;
    mPosY = y;

    //Create the necessary SDL_Rects
    mColliders.resize( 11 );

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;

    //Initialize the collision boxes' width and height (where come from these numbers?)    
    mColliders[ 0 ].w = 6;
    mColliders[ 0 ].h = 1;

    mColliders[ 1 ].w = 10;
    mColliders[ 1 ].h = 1;

    mColliders[ 2 ].w = 14;
    mColliders[ 2 ].h = 1;

    mColliders[ 3 ].w = 16;
    mColliders[ 3 ].h = 2;

    mColliders[ 4 ].w = 18;
    mColliders[ 4 ].h = 2;

    mColliders[ 5 ].w = 20;
    mColliders[ 5 ].h = 6;

    mColliders[ 6 ].w = 18;
    mColliders[ 6 ].h = 2;

    mColliders[ 7 ].w = 16;
    mColliders[ 7 ].h = 2;

    mColliders[ 8 ].w = 14;
    mColliders[ 8 ].h = 1;

    mColliders[ 9 ].w = 10;
    mColliders[ 9 ].h = 1;

    mColliders[ 10 ].w = 6;
    mColliders[ 10 ].h = 1;

    //Initialize colliders relative to position
    //Perhaps some function using mPosX, mPosY?
    shiftColliders();
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

void Dot::move( std::vector<SDL_Rect>& otherColliders )
{
    //Move the dot left or right; Collider also moves
    mPosX += mVelX;
    shiftColliders();

    //If the dot collided the wall, or went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > SCREEN_WIDTH ) || checkCollision( mColliders, otherColliders ) )
    {
        //Move back
        mPosX -= mVelX;
        shiftColliders();
    }

    //Move the dot up or down; Collider also moves
    mPosY += mVelY;
    shiftColliders();

    //If the dot collided the wall, or went too far to the up or down
    if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > SCREEN_HEIGHT ) || checkCollision( mColliders, otherColliders ) )
    {
        //Move back
        mPosY -= mVelY;
        shiftColliders();
    }

}

//Moves the collision boxes relative to the dot's offset
void Dot::shiftColliders()
{
    //The row offset
    int r = 0;

    //Go through the dot's collision boxes
    for( unsigned int set = 0; set < mColliders.size(); ++set )
    {
        //Set x value of collider box, from left-top corner offset
        mColliders[ set ].x = mPosX + ( DOT_WIDTH - mColliders[ set ].w ) / 2;

        //Set y value of collider box, from left-top corner offset
        mColliders[ set ].y = mPosY + r;

        //Move the row offset down the height of the collision box
        r += mColliders[ set ].h;
    }

}



void Dot::render()
{
    //Show the dot
    gDotTexture.render( mPosX, mPosY );
}

std::vector<SDL_Rect>& Dot::getColliders()
{
    return mColliders;
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
        gWindow = SDL_CreateWindow( "SDL Test_28_per-pixel_collision", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
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

    if( !gDotTexture.loadFromFile( "26_motion/dot.bmp" ) )
    {
        printf( "Failed to load dot texture!\n" );
        success = false;
    }

    return success;
}

void close()
{
    //Free loaded image
    gDotTexture.free();

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


//Box collision detector
bool checkCollision( std::vector<SDL_Rect>& a, std::vector<SDL_Rect>& b )
{
    bool collision = false;

    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Go through the A collider boxes
    for (unsigned int i = 0; i < a.size(); ++i)
    {
        leftA = a[ i ].x;
        rightA = a[ i ].x + a[ i ].w;
        topA = a[ i ].y;
        bottomA = a[ i ].y + a[ i ].h;

        //Go through the B collider boxes
        for (unsigned int j = 0; j < b.size(); ++j)
        {
            leftB = b[ j ].x;
            rightB = b[ j ].x + b[ j ].w;
            topB = b[ j ].y;
            bottomB = b[ j ].y + b[ j ].h;

            // if not satisfy the uncollide condition, thus collide
            if ( ( ( rightA <= leftB ) || ( leftA >= rightB ) || ( topA >= bottomB ) || ( bottomA <= topB ) ) == false )
            {
                collision = true;
                return collision;
            }
        }
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

            //The dot that will be moving around on the screen
            Dot dot( 0, 0 );

            //The dot that will be collided against
            Dot otherDot( SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4 );

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

                    //Handle input for the dot
                    dot.handleEvent( e );
                }

                //Move the dot
                dot.move( otherDot.getColliders() );
                
                //Clear screen
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( gRenderer );

                //Render objects
                dot.render();
                otherDot.render();

                //Update screen
                SDL_RenderPresent( gRenderer );
            }
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
