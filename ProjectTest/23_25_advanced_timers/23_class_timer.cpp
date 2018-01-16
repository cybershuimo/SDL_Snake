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

        //The ticks stored when the timer was paused (i.e. duration)
        Uint32 mPausedTicks;

        //The timer status
        bool mPaused;
        bool mStarted;
};

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
    //Start the timer
    mStarted = true;

    //Unpause timer (unecessary?)
    // mPaused = false;

    //Get the current clock time
    mStartTicks = SDL_GetTicks();
    // mPausedTicks = 0;
}

void LTimer::stop()
{
    if ( mStarted )
    {
        mStarted = false;
    }
    mDuration += SDL_GetTicks() - mStartTicks;

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

