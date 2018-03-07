//Snake head file
//Class prototypes and global variable declaration

#ifndef SNAKE_H_
#define SNAKE_H_

//Using SDL, SDL_image, standard IO, and strings
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <sstream>  //std::stringstream needed

//check memory leak
#include <vld.h>

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

//Basic box object class
class Tile
{
    public:
        //The dimensions of the snake body tile
        // static const int TILE_WIDTH = 20;
        // static const int TILE_HEIGHT = 20;

        //Initializes
        Tile( int x, int y );

        //Sets position
        void setPosition( int x, int y );

        //Sets size
        void setSize( int w, int h );

        //Get the collision box
        SDL_Rect getBox();

    private:
        //The attributes of the tile
        SDL_Rect mBox;
};


//SnakeBody object class come from Tile
class SnakeBody : public Tile
{
    public:
        //Initializes
        SnakeBody( int x, int y );

        //Moves snake body
        void move( int &posX, int &posY, SDL_Rect newRect, SDL_Rect headRect, bool &gameOverFlag );

        //Show snake body tile
        void render();
};

//Food object class come from Tile
class Food : public Tile
{
    public:
        //Initializes
        Food( int x, int y );

        //Appear at random position
        void generate();

        //Show food tile
        void render();

        //Disappear if touched by snake head
        bool eaten( SDL_Rect snakeHead );
};

//Button object class come from Tile
class Button : public Tile
{
    public:
        enum ButtonType
        {
            BUTTON_ON,
            BUTTON_OFF,
            BUTTON_TOTAL
        };

        //Initializes
        Button( int x, int y );

        //Handles mouse event
        void handleEvent( SDL_Event& e, bool &flag );

        //Change external flag
        void changeFlag( bool &flag );

        //Set type of button
        void setButtonType( ButtonType type );
    
        //Shows button sprite
        void render();

    private:
        //The attributes of the tile
        //static ButtonType mButtonType;
        ButtonType mButtonType;
};

//The snake that will move around on the screen
class Snake
{
    public:
        //The dimensions of the snake
        static const int SNAKE_WIDTH = 20;
        static const int SNAKE_HEIGHT = 20;

        //Maximum axis velocity of the snake
        static const int SNAKE_VEL = 1;

        //Initializes the variables and allocates particles
        Snake();

        // //Deallocates particles
        // ~Snake();

        //Restart to default status
        //void restart();

        //Takes key presses and adjusts the snake's velocity
        void handleEvent( SDL_Event& e );

        //Moves the snake
        void move( int &posX, int &posY, bool &gameOverFlag );

        //Shows the snake on the screen
        void render();

        //Add one body tile after eating food
        void addLength();

        //Gets length of snake body
        int getLength();

        //Get the collision box
        SDL_Rect getBox();

        //Sets position; used only for game restart
        void restart();

    private:
        //Collision box of the snake
        SDL_Rect mBox;

        //The velocity of the snake
        int mVelX, mVelY;

        //The body length of the snake
        int mBody;
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

class UI
{
        LTimer liveTime;
        bool mStarted;

        int numFoodEaten;

        std::stringstream liveTimeText;
        std::stringstream numFoodEatenText;
        SDL_Color textColor = { 93, 188, 210 };

    public:
        //Initializes variables
        UI();

        //Update numFoodEaten
        void updateFoodEaten();

        //Render on the screen
        void render();

        //Start the timer
        void start();
        
        //When game over, stop the timer
        void paused();

        //Restart variables after restarting game
        void restart();

        //Check status of the timer
        bool isStarted();
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Box collision detector
bool checkCollision( SDL_Rect a, SDL_Rect b );

// Global variables
//Screen dimension constants
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

//The window we'll be rendering to
extern SDL_Window* gWindow;

//The window renderer
extern SDL_Renderer* gRenderer;

//Globally used font
extern TTF_Font* gFont;

//Texture to render
extern LTexture gSnakeTexture;
extern LTexture gBodyTexture;
extern LTexture gFoodTexture;
extern LTexture gTextTexture;
extern LTexture gButtonOnTexture;
extern LTexture gButtonOffTexture;
extern LTexture gUITexture[ 2 ];

#endif