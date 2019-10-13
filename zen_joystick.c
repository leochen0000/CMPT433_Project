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


//------------ functions ---------------------------------------
//***** static functions ******************************
static void assert_zenJoystickButton_OK (enum zenJoystickButton button)
{
    assert((button >= JOYSTICK_UP) && (button < JOYSTICK_MAXBUTTONS));
}



//***** public functions ******************************

// Check if a button is pressed.
// Return value:  true = pressed, false = not pressed, or unsuccessful file I/O.
//
// This function assumes that the GPIO pins have been
// programmed such that 0 = pressed, 1 = not pressed.
//
// NOTE:
// Somehow, opening a FILE * array in an "init" routine and
// then using a FILE * from the array to read a GPIO file does not work
// (fgets() returns NULL).
// Therefore, file is explicitly opened and closed in this routine.
//
_Bool zenJoystickButtonPressed (enum zenJoystickButton button)
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


// Name of button
const char *zenJoystickButtonName (enum zenJoystickButton button)
{
    return(joybuttons[button].namestr);
}


