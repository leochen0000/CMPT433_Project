// ext_buttons.h
// Routines/definitions for 3 external push buttons
// by Dane Lim

#ifndef EXT_BUTTONS_H
#define EXT_BUTTONS_H

#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

//------- Enums and definitions --------------------
// Push-buttons
enum extPushButton
{
    PUSHBUTTON_SELECT = 0,
    PUSHBUTTON_ENTER_EXIT,
    PUSH_MAXBUTTONS
};


//------- function prototypes ----------------------
// Initialize external push-buttons.
void extPushButtonInit ();  // Must be called before using functions

// Check if a button is pushed.
// Return value:  true = pushed, false = not pushed, or unsuccessful file I/O.
//
_Bool extPushButtonPushed (enum extPushButton button);

// Check if a button is pressed ("not pushed" to "pushed").
// Return value:  true = pressed, false = not pressed
//
_Bool extPushButtonPressed (enum extPushButton button);

// Check if a button is released ("pushed" to "not pushed").
// Return value:  true = released, false = not released
//
_Bool extPushButtonReleased (enum extPushButton button);

// Get name string of button
const char *extPushButtonName (enum extPushButton button);

#endif

