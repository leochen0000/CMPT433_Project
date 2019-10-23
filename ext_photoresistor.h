// ext_photoresistor.h
// Routines/definitions for a photoresistor

#ifndef EXT_PHOTORESISTOR_H
#define EXT_PHOTORESISTOR_H

#include <stdbool.h>
#include <assert.h>
#include <stdio.h>


//------- function prototypes ----------------------
// Read A2D value of potentiometer.
// Return value:  0 to 4095 (inclusive), or <0 if error occurred.
int extPhotoresistorReadA2DValue();

// Read voltage of potentiometer.
// Return value:  0 to 1.8V (inclusive), or <0 if error occurred.
double extPhotoresistorReadVoltage();

#endif

