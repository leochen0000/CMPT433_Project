// bbg_led.h
// Routines/definitions for BeagleBone Green's 4 LEDs

#ifndef BBG_LED_H
#define BBG_LED_H

//  Routines/definitions for BeagleBone Green's LEDs
#include <stdio.h>
#include <stdbool.h>

//------- Enums and definitions --------------------
// LED selection
enum bbgLED
{
    BBG_LED0 = 0,
    BBG_LED1,
    BBG_LED2,
    BBG_LED3,
    MAX_BBG_LEDS
};

// LED trigger mode
enum bbgLEDTrigMode
{
    NONE = 0,
    TIMER,
    HEARTBEAT
};

// LED on/off value
enum bbgLEDOffOn
{
    BBG_LED_OFF = 0,
    BBG_LED_ON
};


//------- function prototypes ----------------------
_Bool bbgLED_trigmode(enum bbgLED led, enum bbgLEDTrigMode trigmode);
_Bool bbgLED_set(enum bbgLED led, enum bbgLEDOffOn ledoffon);
_Bool bbgLED_setall(enum bbgLEDOffOn ledoffon);

#endif

