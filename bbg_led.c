#include "bbg_led.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

//------------ variables and definitions -----------------------
// LED filename definitions
#define LEDS_FILEPATH_PREFIX   "/sys/class/leds/beaglebone:green:"
static char *led_foldersel[4] =
{
    "usr0/",
    "usr1/",
    "usr2/",
    "usr3/"
};

// LED trigger strings
static char *led_trigstring[3] =
{
    "none",
    "timer",
    "heartbeat"
};



//------------ functions ---------------------------------------
//***** static functions ******************************
static void assert_bbgLED_OK (enum bbgLED led)
{
    assert((led >= BBG_LED0) && (led <= BBG_LED3));
}

static void assert_bbgLED_TrigMode_OK (enum bbgLEDTrigMode trigmode)
{
    assert((trigmode >= NONE) && (trigmode <= HEARTBEAT));
}

static void assert_bbgLED_OffOn_OK (enum bbgLEDOffOn ledoffon)
{
    assert((ledoffon >= BBG_LED_OFF) && (ledoffon <= BBG_LED_ON));
}


//***** public functions ******************************

// Initialize an LED trigger mode.
// Return value:  true=successful, false=not successful
_Bool bbgLED_trigmode(enum bbgLED led, enum bbgLEDTrigMode trigmode)
{
    char filepath[128];

    // Parameters range checking
    assert_bbgLED_OK(led);
    assert_bbgLED_TrigMode_OK(trigmode);

    // Compose filename
    filepath[0] = 0;
    strcat(filepath, LEDS_FILEPATH_PREFIX);
    strcat(filepath, led_foldersel[led]);
    strcat(filepath, "trigger");
    FILE *LedTriggerFile = fopen(filepath, "w");
    if (LedTriggerFile == NULL) {
       printf("ERROR OPENING %s.\r\n", filepath);
       return false;
    }

    // Write to "trigger" file
    fprintf(LedTriggerFile, "%s", led_trigstring[trigmode]);

    // Make sure to close file
    fclose(LedTriggerFile);
    return true;
}


// LED on/off
// Return value:  true=successful, false=not successful
_Bool bbgLED_set(enum bbgLED led, enum bbgLEDOffOn ledoffon)
{
    char filepath[128];

    // Parameters range checking
    assert_bbgLED_OK(led);
    assert_bbgLED_OffOn_OK(ledoffon);

    // Compose filename
    filepath[0] = 0;
    strcat(filepath, LEDS_FILEPATH_PREFIX);
    strcat(filepath, led_foldersel[led]);
    strcat(filepath, "brightness");
    FILE *LedBrightnessFile = fopen(filepath, "w");

    if (LedBrightnessFile == NULL) {
       printf("ERROR OPENING %s.\r\n", filepath);
       return false;
    }

    // Write to "brightness" file
    fprintf(LedBrightnessFile, "%d", ledoffon);

    // Make sure to close file
    fclose(LedBrightnessFile);
    return true;
}


// All LEDs on/off
// Return value:  true=successful, false=not successful
_Bool bbgLED_setall(enum bbgLEDOffOn ledoffon)
{
    for (int k = 0; k < MAX_BBG_LEDS; k++) {
        if (bbgLED_set(k, ledoffon) == false)
            return false;
    }

    return true;
}

