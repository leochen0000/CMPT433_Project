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


//------- Enums and definitions --------------------
// Selection of left/right digit
enum zenSegDigit
{
	SEGDIGIT_LEFT = 0,
	SEGDIGIT_RIGHT,
	SEGDIGIT_BOTH
};



//------- function prototypes ----------------------
_Bool zenSegDisplayInit();              // Must be called before using functions

_Bool zenSegDisplayDigitOn(enum zenSegDigit digit);
_Bool zenSegDisplayDigitOff(enum zenSegDigit digit);
_Bool zenSegDisplayNumDigit(int num);

#endif

