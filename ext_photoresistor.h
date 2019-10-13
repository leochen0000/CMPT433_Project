// ext_photoresistor.h
// Routines/definitions for Zen Cape's potentiometer

#ifndef EXT_PHOTORESISTOR_H
#define EXT_PHOTORESISTOR_H

#include <stdbool.h>
#include <assert.h>
#include <stdio.h>


//------- function prototypes ----------------------
int extPhotoresistorReadA2DValue();
double extPhotoresistorReadVoltage();

#endif

