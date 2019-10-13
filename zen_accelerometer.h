#ifndef ZEN_ACCELEROMETER_H
#define ZEN_ACCELEROMETER_H

#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

//  Routines/definitions for Zen Cape's 2x15-segment LED display

//------- Enums and definitions --------------------


//------- function prototypes ----------------------
_Bool zenAccelerometerInit();              // Must be called before using functions
_Bool zenAccelerometerRead(int *xval, int *yval, int *zval);

#endif

