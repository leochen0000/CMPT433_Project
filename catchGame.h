// catchGame.h
// Module to start/stop the "Catch" game thread and provide routines to retrieve game data.

#ifndef _CATCHGAME_H_
#define _CATCHGAME_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "zen_joystick.h"
#include "zen_buzzer.h"
#include "ext_8x8led.h"

//------- function prototypes ----------------------

// Start the game thread
//
void catchGame_start(void);

// Stop the game thread
//
void catchGame_stop(void);

// Get game data
// *attempt = number of balls dropped
// *catch = number of balls caught
// *ballx = current ball X position (0 to 7), 0 is leftmost, 7 is rightmost LED dot
// *bally = current ball Y position (0 to 7), 0 is uppermost, 7 is lowermost LED dot
// *catchx = current catcher X position (0 to 7), 0 is leftmost, 7 is rightmost LED dot
//           (catcher Y position is fixed at 7)
//
void catchGame_GetData(int *attempt, int *catch, int *ballx, int *bally, int *catchx);

#endif
