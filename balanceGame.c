// balanceGame.c
// Module to start/stop the "Balance" game thread and provide routines to retrieve game data.
#include "balanceGame.h"

//------------ variables and definitions -----------------------
static pthread_t balanceGame_id;
static pthread_mutex_t balanceGameStat = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t balanceGameData = PTHREAD_MUTEX_INITIALIZER;
static int xpos, ypos;

extern unsigned char font8x8[];

//------------ functions ---------------------------------------
//***** static functions ******************************

int combine(int x, int y) {
	int times = 1;
	while (times <= y) {
		times *= 10;
	}
	return x*times + y;
}

//-------------------------------------------------------
// "Balance the ball" game.
//-------------------------------------------------------
static void *balanceGameThread()
{
    struct timespec reqDelay;
	int xval, yval, zval;

	// Draw ball in center of matrix
	extLED8x8FillPixel(0);
	xpos = 3;
	ypos = 3;
	extLED8x8DrawPixel(xpos, ypos, 1);
	extLED8x8DisplayUpdate();

	// Initalize seg display. 
	// Bottom Left pixel is (0,0) top right pixel is (8,8)
	zenSegDisplayUpdateNum(combine((xpos + 1), (ypos + 1)));

    reqDelay.tv_sec = 0;
   	reqDelay.tv_nsec = 100000000;  // 100ms sampling rate

	while (1) {
		zenAccelerometerRead(&xval, &yval, &zval);

		pthread_mutex_lock(&balanceGameData);
		{
			if (xval > 128) {
				if (xpos < 7) {
					xpos++;
				}
			}
			else if (xval < -128) {
				if (xpos > 0) {
					xpos--;
				}
			}

			if (yval < -128) {
				if (ypos < 7) {
					ypos++;
				}
			}
			else if (yval > 128) {
				if (ypos > 0) {
					ypos--;
				}
			}

			zenSegDisplayUpdateNum(combine((xpos + 1), (ypos + 1))); // Update x y coordinates to seg display
		}
		pthread_mutex_unlock(&balanceGameData);

		extLED8x8FillPixel(0);
		extLED8x8DrawPixel(xpos, ypos, 1);
		extLED8x8DisplayUpdate();

		// sampling delay
		nanosleep(&reqDelay, (struct timespec *) NULL);

		// Check for request to terminate thread
		int lockstat;
		lockstat = pthread_mutex_trylock(&balanceGameStat);
		if (lockstat == 0) {  // lock acquired (0) means terminate
			pthread_exit(0);
		}
	}

	pthread_exit(0);
}





//***** public functions ******************************

void balanceGame_start (void)
{
    printf("Starting game of 'Balance'\n");

	// 3, 2, 1 count down
    extLED8x8CountDown321(font8x8);

	pthread_mutex_init(&balanceGameStat, NULL);

	// Lock mutex used by thread to check for request to end the thread
	pthread_mutex_lock(&balanceGameStat);

	// Start the thread
	pthread_create(&balanceGame_id, NULL, &balanceGameThread, NULL);
}


void balanceGame_stop (void)
{
	// Unlock mutex to let sorting thread know about request to finish
	pthread_mutex_unlock(&balanceGameStat);

    printf("Stopping game of 'Balance'\n");

	// Exit game logo
    extLED8x8ExitGame(font8x8);

	// Wait for thread to finish
	pthread_join(balanceGame_id, NULL);
}


void balanceGame_GetData (int *ballx, int *bally)
{
	pthread_mutex_lock(&balanceGameData);
	{
		*ballx = xpos;
		*bally = ypos;
	}
	pthread_mutex_unlock(&balanceGameData);
}


