//35_window_events, resizable windows, new events to handle

//Using SDL, SDL_image, standard IO, and strings
#include <iostream>
// #include <windows.h>    // for Sleep()

#include <SDL.h>
#include <SDL_image.h>
// #include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>

//check memory leak
#include <vld.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Total windows
const int TOTAL_WINDOWS = 3;

//window class, used as a wrapper for the SDL_Window
class LWindow
{
    public:
        //Intializes internals
        LWindow();

        //Creates window
        bool init();

        //Handles window events
        void handleEvent( SDL_Event& e );

        //Focuses on window
        void focus();

        //Shows windows contents
        void render();

        //Deallocates internals
        void free();

        //Window dimensions
        int getWidth();
        int getHeight();

        //Window focii
        bool hasMouseFocus();
        bool hasKeyboardFocus();
        bool isMinimized();
        bool isShown();

    private:
        //Window data (one renderer per window)
        SDL_Window* mWindow;
        SDL_Renderer* mRenderer;
        int mWindowID;

        //Window dimensions
        int mWidth;
        int mHeight;

        //Window focus
        bool mMouseFocus;
        bool mKeyboardFocus;
        bool mFullScreen;
        bool mMinimized;
        bool mShown;
};


//Starts up SDL and creates window
bool init();

// //Loads media
// bool loadMedia();

//Frees media and shuts down SDL
void close();

//Our custom windows
LWindow gWindows[ TOTAL_WINDOWS ];


//a constructor
LWindow::LWindow()
{
	////Initialize non-existant window
    mWindow = NULL;
    mRenderer = NULL;

    //Window dimensions
    mWidth = 0;
    mHeight = 0;

    //Window focus
    mMouseFocus = false;
    mKeyboardFocus = false;
    mFullScreen = false;
    mMinimized = false;
    mShown = false;
}

//a initializer that creates the window
bool LWindow::init()
{
	//Create window with the SDL_WINDOW_RESIZABLE flag
	mWindow = SDL_CreateWindow( "SDL Test", 
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );

	if( mWindow == NULL )
    {
        printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
    }
    else
    {
    	mMouseFocus = true;
        mKeyboardFocus = true;	// what for?

    	mWidth = SCREEN_WIDTH;
    	mHeight = SCREEN_HEIGHT;

        //Create renderer for window
        mRenderer = SDL_CreateRenderer( mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
        if ( mRenderer == NULL )
        {
            printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
            SDL_DestroyWindow( mWindow );
            mWindow = NULL;
        }
        else
        {
            //Initialize renderer color
            SDL_SetRenderDrawColor( mRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

            //Grab window identifier
            mWindowID = SDL_GetWindowID( mWindow );

            //Flag as opened
            mShown = true;
        }
    }

    return mWindow != NULL && mRenderer != NULL;
}


//an event handler
void LWindow::handleEvent( SDL_Event& e )
{
	if ( e.type == SDL_WINDOWEVENT && e.window.windowID == mWindowID )
	{
		//Caption update flag
        bool updateCaption = false;
        switch( e.window.event )
        {
            //Window appeared
            case SDL_WINDOWEVENT_SHOWN:
            mShown = true;
            break;

            //Window disappeared; window has been hidden
            case SDL_WINDOWEVENT_HIDDEN:
            mShown = false;
            break;

            //if window size change, get new dimensions and repaint
        	case SDL_WINDOWEVENT_SIZE_CHANGED:
        	mWidth = e.window.data1;
            mHeight = e.window.data2;
            SDL_RenderPresent( mRenderer );
        	break;

        	//if exposed (window was obscured in some way and now is not obscured so we want to repaint the window), repaint
        	case SDL_WINDOWEVENT_EXPOSED:
            SDL_RenderPresent( mRenderer );
            break;

            //if Mouse entered window
            case SDL_WINDOWEVENT_ENTER:
            mMouseFocus = true;
            updateCaption = true;
            break;

            //Mouse left window
            case SDL_WINDOWEVENT_LEAVE:
            mMouseFocus = false;
            updateCaption = true;
            break;

            //Window has keyboard focus
            case SDL_WINDOWEVENT_FOCUS_GAINED:
            mKeyboardFocus = true;
            updateCaption = true;
            break;

            //Window lost keyboard focus
            case SDL_WINDOWEVENT_FOCUS_LOST:
            mKeyboardFocus = false;
            updateCaption = true;
            break;

            //Window minimized
            case SDL_WINDOWEVENT_MINIMIZED:
            mMinimized = true;
            break;

            //Window maxized
            case SDL_WINDOWEVENT_MAXIMIZED:
            mMinimized = false;
            break;
            
            //Window restored
            case SDL_WINDOWEVENT_RESTORED:
            mMinimized = false;
            break;

            //Hide on close
            case SDL_WINDOWEVENT_CLOSE:
            SDL_HideWindow( mWindow );
            break;
        }

        //Update window caption with new data
        if( updateCaption )
        {
            std::stringstream caption;
            caption << "SDL Tutorial - ID: " << mWindowID << " MouseFocus: " << ( ( mMouseFocus ) ? "On" : "Off" ) << " KeyboardFocus: " << ( ( mKeyboardFocus ) ? "On" : "Off" );
            SDL_SetWindowTitle( mWindow, caption.str().c_str() );
        }
	}

	//return key Enter: exit or on full screen
    else if( e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN && e.window.windowID == mWindowID )
    {
        if( mFullScreen )
        {
            SDL_SetWindowFullscreen( mWindow, SDL_FALSE );	//?, not flags SDL_WINDOW_FULLSCREEN, SDL_WINDOW_FULLSCREEN_DESKTOP or 0?
            mFullScreen = false;
        }
        else
        {
            SDL_SetWindowFullscreen( mWindow, SDL_TRUE );
            mFullScreen = true;
            mMinimized = false;
        }
    }
}


void LWindow::focus()
{
    //Restore window if needed
    if( !mShown )
    {
        SDL_ShowWindow( mWindow );
    }

    //Move window forward
    //raise a window above other windows and set the input focus
    SDL_RaiseWindow( mWindow );
}

// Render only if the window is not minimized
void LWindow::render()
{
    if( !mMinimized )
    {    
        //Clear screen
        SDL_SetRenderDrawColor( mRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( mRenderer );

        //Update screen
        SDL_RenderPresent( mRenderer );
    }
}

int LWindow::getWidth()
{
    return mWidth;
}

int LWindow::getHeight()
{
    return mHeight;
}

bool LWindow::hasMouseFocus()
{
    return mMouseFocus;
}

bool LWindow::hasKeyboardFocus()
{
    return mKeyboardFocus;
}

bool LWindow::isMinimized()
{
    return mMinimized;
}

bool LWindow::isShown()
{
    return mShown;
}


void LWindow::free()
{
    SDL_DestroyWindow( mWindow );
	mWindow = NULL;
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
        //Create window 0
        if( !gWindows[ 0 ].init() )
        {
            printf( "Window 0 could not be created!\n" );
            success = false;
        }
    }

    return success;
}


void close()
{   
    //Destroy windows
    for ( int i = 0; i < TOTAL_WINDOWS; ++i )
    {
        gWindows[ i ].free();
    }
    

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
        //Initialize the rest of the windows
        for( int i = 1; i < TOTAL_WINDOWS; ++i )
        {
            gWindows[ i ].init();
        }

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

                //Handle window events
                for (int i = 0; i < TOTAL_WINDOWS; ++i)
                {
                    gWindows[ i ].handleEvent( e );
                }

                //Pull up window
                if( e.type == SDL_KEYDOWN )
                {
                    switch( e.key.keysym.sym )
                    {
                        case SDLK_1:
                        gWindows[ 0 ].focus();
                        break;

                        case SDLK_2:
                        gWindows[ 1 ].focus();
                        break;
                            
                        case SDLK_3:
                        gWindows[ 2 ].focus();
                        break;
                    }
                }

            }

            //Update all windows
            for (int i = 0; i < TOTAL_WINDOWS; ++i)
            {
                gWindows[ i ].render();
            }

            //Check all windows
            bool allWindowsClosed = true;
            for( int i = 0; i < TOTAL_WINDOWS; ++i )
            {
                if( gWindows[ i ].isShown() )
                {
                    allWindowsClosed = false;
                    break;
                }
            }

            //Application closed all windows
            if( allWindowsClosed )
            {
                quit = true;
            }

        }   
    }

    //Free resources and close SDL
    close();

    return 0;
}