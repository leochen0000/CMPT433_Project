// zen_segdisplay.h
// Routines/definitions for Zen Cape's 2x15-segment LED display

#ifndef ZEN_SEGDISPLAY_H
#define ZEN_SEGDISPLAY_H

#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>


//------- Enums and definitions --------------------
// Selection of left/right digit
enum zenSegDigit
{
	SEGDIGIT_LEFT = 0,
	SEGDIGIT_RIGHT,
	SEGDIGIT_BOTH
};



//------- function prototypes ----------------------

// Initializes 2-digit x 15-segment LED display
// Return value:  true=success, false=fail
//
_Bool zenSegDisplayInit();              // Must be called before using functions

// Turn on 15-segment digits
// Return value:  true=success, false=fail
//
_Bool zenSegDisplayDigitOn(enum zenSegDigit digit);

// Turn off 15-segment digits
// Return value:  true=success, false=fail
//
_Bool zenSegDisplayDigitOff(enum zenSegDigit digit);

// Display a numeric digit.
// 0 <= num <= 9.
// Return value:  true=success, false=fail
//
_Bool zenSegDisplayNumDigit(int num);

#endif

