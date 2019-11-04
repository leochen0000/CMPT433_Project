// zen_joystick.h
// Routines/definitions for Zen Cape's joystick

#ifndef ZEN_JOYSTICK_H
#define ZEN_JOYSTICK_H

#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

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
// Initialize Zen joystick buttons.
void zenJoystickInit ();  // Must be called before using functions

// Check if a button is pushed.
// Return value:  true = pushed, false = not pushed, or unsuccessful file I/O.
//
_Bool zenJoystickButtonPushed (enum zenJoystickButton button);

// Check if a button is pressed ("not pushed" to "pushed").
// Return value:  true = pressed, false = not pressed, or unsuccessful file I/O.
//
_Bool zenJoystickButtonPressed (enum zenJoystickButton button);

// Check if a button is released ("pushed" to "not pushed").
// Return value:  true = released, false = not released, or unsuccessful file I/O.
//
_Bool zenJoystickButtonPressed (enum zenJoystickButton button);

// Get name string of button
const char *zenJoystickButtonName (enum zenJoystickButton button);

#endif

