
#ifndef _SIMONGAME_H_
#define _SIMONGAME_H_

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

void simonGame_start(void);
void simonGame_stop(void);

#endif