#include "catchGame.h"

//------------ variables and definitions -----------------------
static pthread_t catchGame_id;
static pthread_mutex_t catchGameStat = PTHREAD_MUTEX_INITIALIZER;


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
	unsigned int currpos = 3;
	unsigned int ballposx, ballposy;
	unsigned int sampcnt = 0;
	unsigned int ballslowness = 6;
	unsigned int dropcount = 0;

	extLED8x8FillPixel(0);
	drawCatcher(currpos, 1);
	extLED8x8DisplayUpdate();

    reqDelay.tv_sec = 0;
   	reqDelay.tv_nsec = 100000000;  // 100ms sampling rate

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
			if ((ballposy != 6) || 
				((ballposy == 6) && (ballposx != (currpos-1)) && (ballposx != (currpos+1))))
				extLED8x8DrawPixel(ballposx, ballposy, 0);

			if (ballposy == 6) {
				if (ballposx == currpos)
					zenBuzzerBeep(440, 100);
				else
					zenBuzzerBeep(110, 300);
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

			extLED8x8DrawPixel(ballposx, ballposy, 1);
			extLED8x8DisplayUpdate();
		}

		// Process catcher
		if (button < JOYSTICK_MAXBUTTONS) {
			if (button == JOYSTICK_LEFT) {
				if (currpos > 1) {
					drawCatcher(currpos--, 0);
					drawCatcher(currpos, 1);
					if ((ballposy == 6) && (ballposx == currpos))
						extLED8x8DrawPixel(ballposx, ballposy, 1);
					extLED8x8DisplayUpdate();
				}
			}
			else if (button == JOYSTICK_RIGHT) {
				if (currpos < 6) {
					drawCatcher(currpos++, 0);
					drawCatcher(currpos, 1);
					if ((ballposy == 6) && (ballposx == currpos))
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
	// Lock mutex used by thread to check for request to end the thread
	pthread_mutex_lock(&catchGameStat);

	// Start the thread
	pthread_create(&catchGame_id, NULL, &catchGameThread, NULL);
}


void catchGame_stop (void)
{
	// Unlock mutex to let sorting thread know about request to finish
	pthread_mutex_unlock(&catchGameStat);

	// Wait for thread to finish
	pthread_join(catchGame_id, NULL);
}

