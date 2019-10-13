// ext_buttons.h
// Routines/definitions for 3 external push buttons

#ifndef EXT_BUTTONS_H
#define EXT_BUTTONS_H

#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

//  Routines/definitions for Zen Cape's joystick

//------- Enums and definitions --------------------
// Push-buttons
enum extPushButton
{
    PUSHBUTTON_LEFT = 0,
    PUSHBUTTON_MIDDLE,
    PUSHBUTTON_RIGHT,
    PUSH_MAXBUTTONS
};


//------- function prototypes ----------------------
_Bool extPushButtonPushed (enum extPushButton button);
_Bool extPushButtonPressed (enum extPushButton button);
_Bool extPushButtonReleased (enum extPushButton button);
const char *extPushButtonName (enum extPushButton button);

#endif

