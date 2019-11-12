#include "simonGame.h"
#include "icons8x8.h"

//------------ variables and definitions -----------------------
static pthread_t simonGame_id;
static pthread_mutex_t simonGameStat = PTHREAD_MUTEX_INITIALIZER;

extern unsigned char font8x8[];

//------------ functions ---------------------------------------
//***** static functions ******************************

//-------------------------------------------------------
// // Check for request to terminate thread
// Return Value: void
//-------------------------------------------------------
static void checkTermination() {
    int lockstat;
    lockstat = pthread_mutex_trylock(&simonGameStat);
    if (lockstat == 0) {  // lock acquired (0) means terminate
        pthread_exit(0);
    }
}


//-------------------------------------------------------
// Display current game generated buffer directions
// Return Value: void
//-------------------------------------------------------
static void displayBuffer(int buff[], int curr_index) {
    struct timespec reqDelay;
    reqDelay.tv_sec = 1;
   	reqDelay.tv_nsec = 0;  // sampling rate

    for (int i = 0; i <= curr_index ; i++) {

        if (buff[i] == 0) {
            extLED8x8LoadImage(&icons8x8[UP_ARROW_ICON]);
        }
        else if (buff[i] == 1){
            extLED8x8LoadImage(&icons8x8[DOWN_ARROW_ICON]);
        }
        else if (buff[i] == 2){
            extLED8x8LoadImage(&icons8x8[LEFT_ARROW_ICON]);
        }
        else if (buff[i] == 3) {
            extLED8x8LoadImage(&icons8x8[RIGHT_ARROW_ICON]);
        }

        extLED8x8DisplayUpdate();
        zenBuzzerBeep(440, 100);

        nanosleep(&reqDelay, (struct timespec *) NULL);
        checkTermination();
    }

}


//-------------------------------------------------------
// Randomly display an arrow direction
// Return Value: JOYSTICK direction value
//-------------------------------------------------------
static int newDirection() {
    int x = rand() % 4;
    zenBuzzerBeep(440, 100);

    switch (x)
    {
    case 0:
        return JOYSTICK_UP;
    
    case 1:
        return JOYSTICK_DOWN;

    case 2:
        return JOYSTICK_LEFT;

    case 3:
        return JOYSTICK_RIGHT;
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
                if (button == 0) {
                    extLED8x8LoadImage(&icons8x8[UP_ARROW_ICON]);
                }
                else if (button == 1){
                    extLED8x8LoadImage(&icons8x8[DOWN_ARROW_ICON]);
                }
                else if (button == 2){
                    extLED8x8LoadImage(&icons8x8[LEFT_ARROW_ICON]);
                }
                else if (button == 3) {
                    extLED8x8LoadImage(&icons8x8[RIGHT_ARROW_ICON]);
                }
                extLED8x8DisplayUpdate();
                return button;
            }
        }
        checkTermination();
    }
    return -1;
}

//-------------------------------------------------------
// "Simon Says" game.
//-------------------------------------------------------
static void *simonGameThread()
{
    enum zenJoystickButton buttonPressed;
    enum zenJoystickButton buttonNext;
    struct timespec reqDelay;
    reqDelay.tv_sec = 1;
   	reqDelay.tv_nsec = 0;  // sampling rate

    int maxLevel = 10001;
    int index = 0;
    int gameBuffer[maxLevel];

    // Init game buffer
    for (int i = 0; i < maxLevel; i++) {
        gameBuffer[i] = -1;
    }

    while(1) {

        // Add new random arrow to game buffer
        buttonNext = newDirection();
        if (buttonNext == -1) {
            printf("ERROR: Invalid new joystick direction.\n");
            pthread_exit(0);
        }
        gameBuffer[index] = buttonNext;

        printf("Next button: %d\n", buttonNext);

        // Show current buffer directions
        displayBuffer(gameBuffer, index);

        // Add user input to user buffer
        _Bool wrong = false;
        for (int i = 0; i <= index ; i++) {        
            buttonPressed = getInput();
            if (buttonPressed == -1) {
                printf("ERROR: Invalid user joystick direction.\n");
                pthread_exit(0);
            }

            // Check and update score
            if (buttonPressed == gameBuffer[i]) {
                zenBuzzerBeep(440, 100);
                printf("Correct!\n");
            }
            else {
                // User input wrong, reset game buffer
                extLED8x8LoadImage(&(icons8x8[40]));
                extLED8x8DisplayUpdate();
                zenBuzzerBeep(110, 300);
                zenBuzzerBeep(110, 300);
                zenBuzzerBeep(110, 300);
                zenBuzzerBeep(110, 300);
                zenBuzzerBeep(110, 300);
                printf("Incorrect! Reset to level 1.\n");

                for (int i = 0; i < maxLevel; i++) {
                    gameBuffer[i] = -1;
                }

                index = 0;
                wrong = true;
                break;
            }
            checkTermination();
        }
        
        if (!wrong) {
            // All user input correct
            extLED8x8LoadImage(&(icons8x8[0]));
            extLED8x8DisplayUpdate();
            zenBuzzerBeep(440, 100);
            zenBuzzerBeep(440, 100);
            zenBuzzerBeep(440, 100);
            zenBuzzerBeep(440, 100);
            index++;
            printf("All correct! Level: %d passed!\n", index);
        }

		// sampling delay
		nanosleep(&reqDelay, (struct timespec *) NULL);
		checkTermination();
	}

	pthread_exit(0);
}

//***** public functions ******************************

void simonGame_start(void)
{
    printf("Starting game of 'Simon'\n");
    
    // 3, 2, 1 count down
    extLED8x8CountDown321(font8x8);

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

    printf("Stopping game of 'Simon'\n");

    // Exit game logo
    extLED8x8ExitGame(font8x8);

	// Wait for thread to finish
	pthread_join(simonGame_id, NULL);
}
