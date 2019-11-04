// catchGame.h
// Module to spawn a thread to listen to check and process Zen joystick and accelerometer inputs.

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

void catchGame_start(void);
void catchGame_stop(void);

#endif
