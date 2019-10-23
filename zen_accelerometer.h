//  zen_accelerometer.h
//  Routines/definitions for Zen Cape's accelerometer

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

//------- Enums and definitions --------------------


//------- function prototypes ----------------------
//*****************************************************
// Initializes Zen Cape accelerometer.
// Return value:  true=success, false=fail
//*****************************************************
_Bool zenAccelerometerInit();              // Must be called before using functions

//*****************************************************
// Read X, Y, Z values from Zen Cape accelerometer.
// Return value:  true=success, false=fail
//*****************************************************
_Bool zenAccelerometerRead(int *xval, int *yval, int *zval);

#endif

