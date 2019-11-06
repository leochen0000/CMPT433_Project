#include "balanceGame.h"

//------------ variables and definitions -----------------------
static pthread_t balanceGame_id;
static pthread_mutex_t balanceGameStat = PTHREAD_MUTEX_INITIALIZER;

extern unsigned char font8x8[];

//------------ functions ---------------------------------------
//***** static functions ******************************

//-------------------------------------------------------
// "Balance the ball" game.
//-------------------------------------------------------
static void *balanceGameThread()
{
    struct timespec reqDelay;
	unsigned int xpos, ypos;
	int xval, yval, zval;

	// 3, 2, 1 count down
    reqDelay.tv_sec = 1;
   	reqDelay.tv_nsec = 0;
	extLED8x8LoadImage(&font8x8['3'*8]);
	extLED8x8DisplayUpdate();
	nanosleep(&reqDelay, (struct timespec *) NULL);
	extLED8x8LoadImage(&font8x8['2'*8]);
	extLED8x8DisplayUpdate();
	nanosleep(&reqDelay, (struct timespec *) NULL);
	extLED8x8LoadImage(&font8x8['1'*8]);
	extLED8x8DisplayUpdate();
	nanosleep(&reqDelay, (struct timespec *) NULL);

	// Draw ball in center of matrix
	extLED8x8FillPixel(0);
	xpos = 3;
	ypos = 3;
	extLED8x8DrawPixel(xpos, ypos, 1);
	extLED8x8DisplayUpdate();

    reqDelay.tv_sec = 0;
   	reqDelay.tv_nsec = 100000000;  // 100ms sampling rate

	while (1) {
		zenAccelerometerRead(&xval, &yval, &zval);

		if (xval > 128) {
			if (xpos < 7)
				xpos++;
		}
		else if (xval < -128) {
			if (xpos > 0)
				xpos--;
		}

		if (yval < -128) {
			if (ypos < 7)
				ypos++;
		}
		else if (yval > 128) {
			if (ypos > 0)
				ypos--;
		}

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
    printf("Starting game of 'Catch'\n");
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

	// Wait for thread to finish
	pthread_join(balanceGame_id, NULL);
}


