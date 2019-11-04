// zen_buzzer.h
// Routines/definitions for Zen Cape's buzzer

#ifndef ZEN_BUZZER_H
#define ZEN_BUZZER_H

#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

//------- Enums and definitions --------------------

//------- function prototypes ----------------------
// Initialize Zen buzzer.
void zenBuzzerInit ();  // Must be called before using functions

// Turn on the buzzer with a specific frequency (Hz).
//
void zenBuzzerOn(long freq);

// Turn off the buzzer.
//
void zenBuzzerOff();

// Turn on the buzzer with a specific frequency (Hz) and duration (ms).
//
void zenBuzzerBeep(long freq, long duration);

#endif

