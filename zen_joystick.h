// zen_joystick.h
// Routines/definitions for Zen Cape's joystick

#ifndef ZEN_JOYSTICK_H
#define ZEN_JOYSTICK_H

#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

//  Routines/definitions for Zen Cape's joystick

//------- Enums and definitions --------------------
// Joystick buttons
enum zenJoystickButton
{
    JOYSTICK_UP = 0,
    JOYSTICK_DOWN,
    JOYSTICK_LEFT,
    JOYSTICK_RIGHT,
    JOYSTICK_ENTER,
    JOYSTICK_MAXBUTTONS
};


//------- function prototypes ----------------------
_Bool zenJoystickButtonPressed (enum zenJoystickButton button);
const char *zenJoystickButtonName (enum zenJoystickButton button);

#endif

