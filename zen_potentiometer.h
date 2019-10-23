// zen_potentiometer.h
// Routines/definitions for Zen Cape's potentiometer

#ifndef ZEN_POTENTIOMETER_H
#define ZEN_POTENTIOMETER_H

#include <stdbool.h>
#include <assert.h>
#include <stdio.h>


//------- function prototypes ----------------------
// Read A2D value of potentiometer.
// Return value:  0 to 4095 (inclusive), or <0 if error occurred.
int zenPotentiometerReadA2DValue();

// Read voltage of potentiometer.
// Return value:  0 to 1.8V (inclusive), or <0 if error occurred.
double zenPotentiometerReadVoltage();

#endif

