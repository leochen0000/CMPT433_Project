// balanceGame.h
// Module to spawn a thread to listen to check and process Zen joystick and accelerometer inputs.

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

#endif
