// snakeGame.h
// Module to start/stop the "Snake" game thread and provide routines to retrieve game data.

#ifndef _SNAKEGAME_H_
#define _SNAKEGAME_H_

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


//------- Enums and definitions --------------------
typedef struct {
	int x;
	int y;
} snakegame_coords;

//------- function prototypes ----------------------

// Start the game thread
//
void snakeGame_start(void);

// Stop the game thread
//
void snakeGame_stop(void);

// Get game data
// *snakebody_length = length of snake's body (number of pixels)
// *snakebodycoords = pointer to array of coordinates for snake's body
// *applecoords = pointer to coordinates of apple
// *apples_eaten = number of apples eaten by snake
//
void snakeGame_GetData (int *snakebody_length, snakegame_coords *snakebodycoords,
						snakegame_coords *applecoord, int *apples_eaten);

#endif
