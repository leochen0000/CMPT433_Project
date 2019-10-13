#include "ext_buttons.h"

//------------ variables and definitions -----------------------
#define GPIO_FILEPATH_PREFIX   "/sys/class/gpio/gpio"

struct PushButtonInfo
{
    const char *namestr;
    const int portnum;
};

static const struct PushButtonInfo pushbuttons[] =
{
    {"LEFT",     31},
    {"MIDDLE",   50},
    {"RIGHT",   48}
};

static _Bool previous_state[PUSH_MAXBUTTONS];


//------------ functions ---------------------------------------
//***** static functions ******************************
static void assert_extPushButton_OK (enum extPushButton button)
{
    assert((button >= PUSHBUTTON_LEFT) && (button < PUSH_MAXBUTTONS));
}

//***** public functions ******************************
// Check if a button is pushed.
// Return value:  true = pressed, false = not pressed, or unsuccessful file I/O.
//
// This function assumes that the GPIO pins have been
// programmed such that 0 = pushed, 1 = not pushed.
//
_Bool extPushButtonPushed (enum extPushButton button)
{
    char filepath[128];

    // Parameters range checking
    assert_extPushButton_OK(button);

    // Compose filename
    sprintf(filepath, "%s%02d/value", GPIO_FILEPATH_PREFIX, pushbuttons[button].portnum);
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
_Bool extPushButtonPressed (enum extPushButton button)
{
	_Bool retval, currval;

	currval = extPushButtonPushed(button);
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
_Bool extPushButtonReleased (enum extPushButton button)
{
	_Bool retval, currval;

	currval = extPushButtonPushed(button);
	if (previous_state[button] && !currval) {  // button released
		retval = true;
	}
	else
		retval = false;

	previous_state[button] = currval;
	return retval;
}


// Name of button
const char *extPushButtonName (enum extPushButton button)
{
    return(pushbuttons[button].namestr);
}


