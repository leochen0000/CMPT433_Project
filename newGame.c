// newGame.c
// Module to start/stop the new game thread and provide routines to retrieve game data.
#include "newGame.h"

//------------ variables and definitions -----------------------
#define BIRDBODY_LENGTH   2

#define MAX_WIDTH   7
#define MIN_WIDTH   0
#define MAX_HEIGHT  0
#define MIN_HEIGHT  7


static pthread_t newGame_id;
static pthread_mutex_t newGameStat = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t newGameData = PTHREAD_MUTEX_INITIALIZER;
static birdgame_coords birdbody[BIRDBODY_LENGTH];  // coordinates of bird's body (pixels)

static _Bool gameStopped = false;
static int walls_passed = 0;
static _Bool bird_in_motion = false;

// static enum zenJoystickButton current_direction;


extern unsigned char font8x8[];

//------------ functions ---------------------------------------
//***** static functions ******************************

static void drawBirdBody()
{
	for (int k = 0; k < BIRDBODY_LENGTH; k++)
		extLED8x8DrawPixel(birdbody[k].x, birdbody[k].y, 1);
}

static void initBirdBody()
{
	pthread_mutex_lock(&newGameData);
	{   
        // head
		birdbody[0].x = 2;  
		birdbody[0].y = 3;

        // tail
		birdbody[1].x = 1;
		birdbody[1].y = 4;
	}
	pthread_mutex_unlock(&newGameData);
}

static void flyUp()
{
    printf("Called flyUp()\n");
    pthread_mutex_lock(&newGameData);
    {   
        // Check that we don't go out of bounds
        if (birdbody[0].y > MAX_HEIGHT) {
            // head 
		    birdbody[0].y--;
            // tail
		    birdbody[1].y--;
            printf("flyUp()\n");
        }
    }
    pthread_mutex_unlock(&newGameData);
}

static void flyDown()
{
    printf("Called flyDown()\n");
    pthread_mutex_lock(&newGameData);
    {   
        // Check that we don't go out of bounds
        if (birdbody[0].y < MIN_HEIGHT) {
            // head 
		    birdbody[0].y++;
            // tail
		    birdbody[1].y++;
            printf("flyDown()\n");
        }
    }
    pthread_mutex_unlock(&newGameData);
}

//-------------------------------------------------------
// New game.
//-------------------------------------------------------
static void *newGameThread()
{
    struct timespec reqDelay;
    enum zenJoystickButton button;
    int samplecnt;

    gameStopped = false;
	walls_passed = 0;
    bird_in_motion = false;

    // Initialize bird's body
    initBirdBody();

	// Draw bird on the left side of matrix
	extLED8x8FillPixel(0);
    drawBirdBody();
	extLED8x8DisplayUpdate();

    // Initialize score on seg display
	zenSegDisplayUpdateNum(walls_passed);

    reqDelay.tv_sec = 0;
   	reqDelay.tv_nsec = 100000000;  // 100ms sampling rate

	while (1) {
		// Sample all joystick buttons
		for (button = JOYSTICK_UP; button < JOYSTICK_MAXBUTTONS; button++) {
			if (zenJoystickButtonPushed (button)) {
				break;
			}
		}

        if (gameStopped == false) {
            if (bird_in_motion == false) {
				if (button < JOYSTICK_MAXBUTTONS) {
                    // If any joystick button is pressed, fly up one unit
					flyUp();

					extLED8x8FillPixel(0);
					drawBirdBody();
					// extLED8x8DrawPixel(apple.x, apple.y, 1);
					extLED8x8DisplayUpdate();

					bird_in_motion = true;
					samplecnt = 0;
				} 
			} 
            else { // Bird is in flight
                if (++samplecnt == 10) {
                    samplecnt = 0;
                    // TODO: Check for collision.
                    if (button < JOYSTICK_MAXBUTTONS) {
                    // If any joystick button is pressed, fly up one unit
					flyUp();

					extLED8x8FillPixel(0);
					drawBirdBody();
					// extLED8x8DrawPixel(apple.x, apple.y, 1);
					extLED8x8DisplayUpdate();

                    } else {
                        flyDown();
                        extLED8x8FillPixel(0);
                        drawBirdBody();
                        extLED8x8DisplayUpdate();
                    }
                }
                
            }
        } 

		// sampling delay
		nanosleep(&reqDelay, (struct timespec *) NULL);

		// Check for request to terminate thread
		int lockstat;
		lockstat = pthread_mutex_trylock(&newGameStat);
		if (lockstat == 0) {  // lock acquired (0) means terminate
			pthread_exit(0);
		}
	}

	pthread_exit(0);
}





//***** public functions ******************************

void newGame_start (void)
{
    printf("Starting game of 'Blank'\n");

	// 3, 2, 1 count down
    // extLED8x8CountDown321(font8x8);

	pthread_mutex_init(&newGameStat, NULL);

	// Lock mutex used by thread to check for request to end the thread
	pthread_mutex_lock(&newGameStat);

	// Start the thread
	pthread_create(&newGame_id, NULL, &newGameThread, NULL);
}


void newGame_stop (void)
{
	// Unlock mutex to let sorting thread know about request to finish
	pthread_mutex_unlock(&newGameStat);

    printf("Stopping game of 'Blank'\n");

	// Exit game logo
    extLED8x8ExitGame(font8x8);

	// Wait for thread to finish
	pthread_join(newGame_id, NULL);
}


// void newGame_GetData (int *ballx, int *bally)
// {
// 	pthread_mutex_lock(&newGameData);
// 	{
// 		*ballx = xpos;
// 		*bally = ypos;
// 	}
// 	pthread_mutex_unlock(&newGameData);
// }


