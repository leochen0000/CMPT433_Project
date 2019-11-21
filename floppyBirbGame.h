// floppyBirbGame.h
// Module to start/stop the Floppy Birb game thread and provide routines to retrieve game data.

#ifndef _FLOPPYBIRBGAME_H_
#define _FLOPPYBIRBGAME_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "zen_joystick.h"
#include "zen_accelerometer.h"
#include "zen_buzzer.h"
#include "ext_8x8led.h"
#include "zen_segdisplay.h"

typedef struct {
	int x;
	int y;
} birdgame_coords;


//------- function prototypes ----------------------

// Start the game thread
//
void floppyBirbGame_start(void);

// Stop the game thread
//
void floppyBirbGame_stop(void);

// Get game data
// *ballx = current ball X position (0 to 7), 0 is leftmost, 7 is rightmost LED dot
// *bally = current ball Y position (0 to 7), 0 is uppermost, 7 is lowermost LED dot
//
// void floppyBirbGame_GetData (int *ballx, int *bally);

#endif
