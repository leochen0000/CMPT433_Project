#include "simonGame.h"
#include "icons8x8.h"

//------------ variables and definitions -----------------------
static pthread_t simonGame_id;
static pthread_mutex_t simonGameStat = PTHREAD_MUTEX_INITIALIZER;

//------------ functions ---------------------------------------
//***** static functions ******************************

//-------------------------------------------------------
// Randomly display an arrow direction
// Return Value: JOYSTICK direction value
//-------------------------------------------------------
static int newDirection() {
    int x = rand() % 4;

    switch (x)
    {
    case 0:
        extLED8x8LoadImage(&icons8x8[RIGHT_ARROW_ICON]);
        extLED8x8DisplayUpdate();
        return JOYSTICK_RIGHT;
    
    case 1:
        extLED8x8LoadImage(&icons8x8[LEFT_ARROW_ICON]);
        extLED8x8DisplayUpdate();
        return JOYSTICK_LEFT;

    case 2:
        extLED8x8LoadImage(&icons8x8[UP_ARROW_ICON]);
        extLED8x8DisplayUpdate();
        return JOYSTICK_UP;

    case 3:
        extLED8x8LoadImage(&icons8x8[DOWN_ARROW_ICON]);
        extLED8x8DisplayUpdate();
        return JOYSTICK_DOWN;
    }
    return -1;
}

//-------------------------------------------------------
// Get user's joystick input
// Return Value: user JOYSTICK direction value
//-------------------------------------------------------
static int getInput() {
    enum zenJoystickButton button;
    // Sample all joystick buttons
    while(1) {
        for (button = JOYSTICK_UP; button < JOYSTICK_MAXBUTTONS; button++) {
            if (zenJoystickButtonPressed(button)) {
                return button;
            }
        }
    }
    return -1;
}

//-------------------------------------------------------
// "Simon Says" game.
//-------------------------------------------------------
static void *simonGameThread()
{
    enum zenJoystickButton buttonPressed;
    enum zenJoystickButton buttonExpected;
    struct timespec reqDelay;
    unsigned int userScore = 0;
    unsigned int totalScore = 0;

    reqDelay.tv_sec = 0;
   	reqDelay.tv_nsec = 100000000;  // 100ms sampling rate

    printf("Starting game Simon Says!\n");

    while(1) {
        // Display random arrow
        buttonExpected = newDirection();

        if (buttonExpected == -1) {
            printf("ERROR: Invalid joystick direction.\n");
            pthread_exit(0);
        }

        // User joystick input
        buttonPressed = getInput();
        if (buttonExpected == -1) {
            printf("ERROR: Invalid joystick direction.\n");
            pthread_exit(0);
        }

        // Check and update score
        if (buttonPressed == buttonExpected) {
            zenBuzzerBeep(440, 100);
            printf("Correct!\n");
            userScore++;
        }
        else {
            zenBuzzerBeep(110, 300);
            printf("Incorrect!\n");            
        }

        totalScore++;
        printf("Your score: %d, Total Score %d\n", userScore, totalScore);
        
		// sampling delay
		nanosleep(&reqDelay, (struct timespec *) NULL);

		// Check for request to terminate thread
		int lockstat;
		lockstat = pthread_mutex_trylock(&simonGameStat);
		if (lockstat == 0) {  // lock acquired (0) means terminate
			pthread_exit(0);
		}
	}

	pthread_exit(0);
}

//***** public functions ******************************

void simonGame_start(void)
{
	pthread_mutex_init(&simonGameStat, NULL);

	// Lock mutex used by thread to check for request to end the thread
	pthread_mutex_lock(&simonGameStat);

	// Start the thread
	pthread_create(&simonGame_id, NULL, &simonGameThread, NULL);
}

void simonGame_stop(void)
{
	// Unlock mutex to let sorting thread know about request to finish
	pthread_mutex_unlock(&simonGameStat);

	// Wait for thread to finish
	pthread_join(simonGame_id, NULL);
}
