#include "zen_joystick.h"

//------------ variables and definitions -----------------------
#define GPIO_FILEPATH_PREFIX   "/sys/class/gpio/gpio"

struct JoystickButtonInfo
{
    const char *namestr;
    const int portnum;
};

static const struct JoystickButtonInfo joybuttons[] =
{
    {"UP",     26},
    {"DOWN",   46},
    {"LEFT",   65},
    {"RIGHT",  47},
    {"ENTER",  27}
};

static _Bool previous_state[JOYSTICK_MAXBUTTONS];

//------------ functions ---------------------------------------
//***** static functions ******************************
static void assert_zenJoystickButton_OK (enum zenJoystickButton button)
{
    assert((button >= JOYSTICK_UP) && (button < JOYSTICK_MAXBUTTONS));
}



//***** public functions ******************************

// Initialize Zen joystick buttons.
void zenJoystickInit ()
{
	char buffstr[128];
	FILE *GPIOFile;
    struct timespec reqDelay;
    reqDelay.tv_sec = 0;
    reqDelay.tv_nsec = 300000000;  // 300ms

	// Export GPIO pins
	for (int k = 0; k < JOYSTICK_MAXBUTTONS; k++) {
		GPIOFile = fopen("/sys/class/gpio/export", "w");
		if (GPIOFile == NULL) {
			printf("ERROR!  Cannot open /sys/class/gpio/export for writing!\n");
			exit(1);
		}

		fprintf(GPIOFile, "%d", joybuttons[k].portnum);
		fclose(GPIOFile);
	}
	nanosleep(&reqDelay, (struct timespec *) NULL);

		// Configure GPIO pins as inputs
	for (int k = 0; k < JOYSTICK_MAXBUTTONS; k++) {
		sprintf(buffstr, "/sys/class/gpio/gpio%d/direction", joybuttons[k].portnum);
		GPIOFile = fopen(buffstr, "w");
		if (GPIOFile == NULL) {
			printf("ERROR!  Cannot open %s for writing!\n", buffstr);
			exit(1);
		}
		fprintf(GPIOFile, "in");
		fclose(GPIOFile);
	}

}


// Check if a button is pressed.
// Return value:  true = pressed, false = not pressed, or unsuccessful file I/O.
//
_Bool zenJoystickButtonPushed (enum zenJoystickButton button)
{
    char filepath[128];

    // Parameters range checking
    assert_zenJoystickButton_OK(button);

    // Compose filename
    sprintf(filepath, "%s%02d/value", GPIO_FILEPATH_PREFIX, joybuttons[button].portnum);
    FILE *GPIOPortFile = fopen(filepath, "r");
    if (GPIOPortFile == NULL) {
       printf("ERROR OPENING %s.\r\n", filepath);
       return false;
    }

    // Read status character
    fgets(filepath, 128, GPIOPortFile);

    // Make sure to close file
    fclose(GPIOPortFile);

    // Check value character
    if (filepath[0] == '0')
       return true;
    else
       return false;
}


// Check if a button is pressed.
// Return value:  true = pressed, false = not pressed
//
_Bool zenJoystickButtonPressed (enum zenJoystickButton button)
{
	_Bool retval, currval;

	currval = zenJoystickButtonPushed(button);
	if (!previous_state[button] && currval) {  // button pressed
		retval = true;
	}
	else
		retval = false;

	previous_state[button] = currval;
	return retval;
}


// Check if a button is released.
// Return value:  true = released, false = not released
//
_Bool zenJoystickButtonReleased (enum zenJoystickButton button)
{
	_Bool retval, currval;

	currval = zenJoystickButtonPushed(button);
	if (previous_state[button] && !currval) {  // button released
		retval = true;
	}
	else
		retval = false;

	previous_state[button] = currval;
	return retval;
}


// Get name string of button
const char *zenJoystickButtonName (enum zenJoystickButton button)
{
    return(joybuttons[button].namestr);
}


