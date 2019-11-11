// balanceGame.h
// Module to start/stop the "Balance" game thread and provide routines to retrieve game data.

#ifndef _BALANCEGAME_H_
#define _BALANCEGAME_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "zen_accelerometer.h"
#include "zen_buzzer.h"
#include "ext_8x8led.h"

//------- function prototypes ----------------------

// Start the game thread
//
void balanceGame_start(void);

// Stop the game thread
//
void balanceGame_stop(void);

// Get game data
// *ballx = current ball X position (0 to 7), 0 is leftmost, 7 is rightmost LED dot
// *bally = current ball Y position (0 to 7), 0 is uppermost, 7 is lowermost LED dot
//
void balanceGame_GetData (int *ballx, int *bally);

#endif
