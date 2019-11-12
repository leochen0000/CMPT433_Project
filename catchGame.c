// catchGame.c
// Module to start/stop the "Catch" game thread and provide routines to retrieve game data.
#include "catchGame.h"

//------------ variables and definitions -----------------------
static pthread_t catchGame_id;
static pthread_mutex_t catchGameStat = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t catchGameData = PTHREAD_MUTEX_INITIALIZER;

static int catches = 0;
static int attempts = 0;
static int currposx = 3;
static int ballposx, ballposy;

extern unsigned char font8x8[];

//------------ functions ---------------------------------------
//***** static functions ******************************

//-------------------------------------------------------
// Draw catcher
//-------------------------------------------------------
static void drawCatcher(unsigned int xpos, unsigned char pixel)
{
	extLED8x8DrawPixel(xpos, 7, pixel);
	extLED8x8DrawPixel(xpos-1, 6, pixel);
	extLED8x8DrawPixel(xpos-1, 7, pixel);
	extLED8x8DrawPixel(xpos+1, 6, pixel);
	extLED8x8DrawPixel(xpos+1, 7, pixel);
}


//-------------------------------------------------------
// "Catch the falling ball" game.
//-------------------------------------------------------
static void *catchGameThread()
{
    enum zenJoystickButton button;
    struct timespec reqDelay;
	unsigned int sampcnt = 0;
	unsigned int ballslowness = 6;
	unsigned int dropcount = 0;

	catches = 0;
	attempts = 0;
	currposx = 3;

	// Draw catcher
	extLED8x8FillPixel(0);
	drawCatcher(currposx, 1);
	extLED8x8DisplayUpdate();

    reqDelay.tv_sec = 0;
   	reqDelay.tv_nsec = 100000000;  // 100ms sampling rate

	// Draw ball
	ballposx = (rand() % 6) + 1;
	ballposy = 0;
	extLED8x8DrawPixel(ballposx, ballposy, 1);

	while (1) {
		// Sample all joystick buttons
		for (button = JOYSTICK_UP; button < JOYSTICK_MAXBUTTONS; button++) {
			if (zenJoystickButtonPushed (button)) {
				break;
			}
		}

		// Process ball dropping
		if (++sampcnt == ballslowness) {
			sampcnt = 0;

			pthread_mutex_lock(&catchGameData);
			{
				if ((ballposy != 6) || 
					((ballposy == 6) && (ballposx != (currposx-1)) && (ballposx != (currposx+1))))
					extLED8x8DrawPixel(ballposx, ballposy, 0);

				if (ballposy == 6) {
					if (ballposx == currposx) {
						zenBuzzerBeep(440, 100);
						catches++;
					}
					else {
						zenBuzzerBeep(110, 300);
					}
					attempts++;
					printf("Attempts=%d, Catches=%d\n", attempts, catches);

					ballposx = (rand() % 6) + 1;
					ballposy = 0;

					// Speed up dropping
					if (++dropcount == 4) {
						dropcount = 0;
						if (ballslowness > 2)
							ballslowness--;
					}

				}
				else {
					ballposy++;
				}
			}
			pthread_mutex_unlock(&catchGameData);

			extLED8x8DrawPixel(ballposx, ballposy, 1);
			extLED8x8DisplayUpdate();
		}

		// Process catcher
		if (button < JOYSTICK_MAXBUTTONS) {
			if (button == JOYSTICK_LEFT) {
				if (currposx > 1) {
					pthread_mutex_lock(&catchGameData);
					{
						drawCatcher(currposx--, 0);
						drawCatcher(currposx, 1);
					}
					pthread_mutex_unlock(&catchGameData);
					if ((ballposy == 6) && (ballposx == currposx))
						extLED8x8DrawPixel(ballposx, ballposy, 1);
					extLED8x8DisplayUpdate();
				}
			}
			else if (button == JOYSTICK_RIGHT) {
				if (currposx < 6) {
					pthread_mutex_lock(&catchGameData);
					{
						drawCatcher(currposx++, 0);
						drawCatcher(currposx, 1);
					}
					pthread_mutex_unlock(&catchGameData);
					if ((ballposy == 6) && (ballposx == currposx))
						extLED8x8DrawPixel(ballposx, ballposy, 1);
					extLED8x8DisplayUpdate();
				}
			}
		}


		// sampling delay
		nanosleep(&reqDelay, (struct timespec *) NULL);

		// Check for request to terminate thread
		int lockstat;
		lockstat = pthread_mutex_trylock(&catchGameStat);
		if (lockstat == 0) {  // lock acquired (0) means terminate
			pthread_exit(0);
		}
	}

	pthread_exit(0);
}





//***** public functions ******************************

void catchGame_start (void)
{
    printf("Starting game of 'Catch'\n");
	
	// 3, 2, 1 count down
    extLED8x8CountDown321(font8x8);

	pthread_mutex_init(&catchGameStat, NULL);

	// Lock mutex used by thread to check for request to end the thread
	pthread_mutex_lock(&catchGameStat);

	// Start the thread
	pthread_create(&catchGame_id, NULL, &catchGameThread, NULL);
}


void catchGame_stop (void)
{
	// Unlock mutex to let sorting thread know about request to finish
	pthread_mutex_unlock(&catchGameStat);

    printf("Stopping game of 'Catch'\n");

	// Exit game logo
    extLED8x8ExitGame(font8x8);

	// Wait for thread to finish
	pthread_join(catchGame_id, NULL);
}


void catchGame_GetData(int *attempt, int *catch, int *ballx, int *bally, int *catchx)
{
	pthread_mutex_lock(&catchGameData);
	{
		*attempt = attempts;
		*catch = catches;
		*ballx = ballposx;
		*bally = ballposy;
		*catchx = currposx;
	}
	pthread_mutex_unlock(&catchGameData);
}



