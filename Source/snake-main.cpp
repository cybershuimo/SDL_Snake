//Main program file
//basic actions, moving, eating food and growing

#include "snake.h"

//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>

//check memory leak
#include <vld.h>


//Snake body length limit
const int BODY_LIMIT = 100;

//The snake that will be moving around on the screen
Snake snake;
//Snake body tiles
SnakeBody* snakeBody[ BODY_LIMIT ];
//Food to be eaten; initialized position
Food food( 20 * 20, 20 * 20 );


int main( int argc, char* args[] )
//int main()
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

            //Play Again button
            Button playAgainButton( 0, 0 );

            int snakeBodyLength = snake.getLength();
            int posX = 360;   // set it as the offset X of snake head 
            int posY = 200;   // set it as the offset Y of snake head

            //Initialize snake body
            for (int i = 0; i < snakeBodyLength; ++i)
            {
                posX += 20;
                snakeBody[ i ] = new SnakeBody( posX, posY );
            }

            //Keeps track of time between steps
            LTimer stepTimer;
            stepTimer.start();

            //Initialize UI
            UI ui;

            //The game over flags
            bool gameOverFlag = false;
            bool restartFlag = false;

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
                    else if ( !gameOverFlag )
                    {
                        //game is being played; Handle input for the snake head
                        snake.handleEvent( e );
                    }
                    else
                    {
                        //game is over; Handle input for the button
                        playAgainButton.handleEvent( e, restartFlag );
                    }   
                }

                // 1.Game playing
                if ( !gameOverFlag )
                {
                    //Move the snake
                    //Every 200ms (this could change) moves one step (= head length)
                    if ( stepTimer.getTicks() > 200 )
                    {
                        //Snake head moves
                        snake.move( posX, posY, gameOverFlag );

                        //Snake body moves
                        snakeBody[ 0 ]->move( posX, posY, snake.getBox(), snake.getBox(), gameOverFlag );
                        for (int i = 1; i < snakeBodyLength; ++i)
                        {
                            snakeBody[ i ]->move( posX, posY, snakeBody[ i - 1 ]->getBox(), snake.getBox(), gameOverFlag );
                        }

                        //Restart timer
                        stepTimer.start();
                    }

                    //Clear screen; black background color
                    SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
                    SDL_RenderClear( gRenderer );

                    //Render objects
                    //snake.render();
                    for (int i = 0; i < snakeBodyLength; ++i)
                    {
                        snakeBody[ i ]->render();
                    }
                    snake.render();

                    //Check if Food eaten or not
                    if ( !food.eaten( snake.getBox() ) )
                    {
                        food.render();
                    }
                    //Eating food
                    else
                    {
                        //Generate a new food
                        food.generate();
                        //Add snake length
                        snake.addLength();
                        snakeBodyLength = snake.getLength();
                        if ( snakeBodyLength <= BODY_LIMIT )
                        {
                            snakeBody[ snakeBodyLength - 1 ] = new SnakeBody( posX, posY );
                        }
                        //Update numFoodEaten on UI
                        ui.updateFoodEaten();   
                    }

                    //Update UI
                    if ( !ui.isStarted() )
                    {
                        ui.start();
                    }
                    ui.render();
                }

                // 2.Game over and the user pressed restart button
                else if ( gameOverFlag && restartFlag )
                {
                    //Deallocate previous snake body tiles
                    for( int i = 0; i < snakeBodyLength; ++i )
                    {
                         if( snakeBody[ i ] != NULL )
                         {
                            delete snakeBody[ i ];
                            snakeBody[ i ] = NULL;
                         }
                    }

                    //Renew snake and body tiles
                    snake.restart();
                    snakeBodyLength = snake.getLength();
                    posX = 360;   // set it as the offset X of snake head 
                    posY = 200;   // set it as the offset Y of snake head

                    //Initialize snake body
                    for (int i = 0; i < snakeBodyLength; ++i)
                    {
                        posX += 20;
                        snakeBody[ i ] = new SnakeBody( posX, posY );
                    }

                    //Clear screen; black background color
                    SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
                    SDL_RenderClear( gRenderer );

                    //Render objects
                    for (int i = 0; i < snakeBodyLength; ++i)
                    {
                        snakeBody[ i ]->render();
                    }
                    snake.render();

                    //Keeps track of time between steps
                    stepTimer.start();

                    //Restart UI
                    ui.restart();

                    //Change the game over flags to default states
                    gameOverFlag = false;
                    restartFlag = false;
                }

                // 3.Game over but not restart yet
                else
                {
                    //Render "Game Over" text
                    gTextTexture.render( ( SCREEN_WIDTH - gTextTexture.getWidth() ) / 2, ( SCREEN_HEIGHT - gTextTexture.getHeight() ) / 2 );
                    //Render "Play Again" button
                    playAgainButton.render();
                    //Render paused UI
                    ui.paused();
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