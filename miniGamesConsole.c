#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bbg_led.h"
#include "zen_joystick.h"
#include "zen_accelerometer.h"
#include "ext_buttons.h"
#include "ext_photoresistor.h"
#include "ext_8x8led.h"
#include "font8x8.h"

int main(int argc, char **argv)
{
    struct timespec reqDelay;
    reqDelay.tv_sec = 1;
    reqDelay.tv_nsec = 250000000;  // 100ms = 100,000us = 100,000,000ns

	// Initialize 8x8 LED display
	if (extLED8x8Init() == false) {
		printf("ERROR!  extLED8x8Init() returned false!\n");
		return 1;
	}

	// Turn on 8x8 LED display
	if (extLED8x8DisplayOn(HT16K33_BLINK_OFF) == false) {
		printf("ERROR!  extLED8x8DisplayOn() returned false!\n");
		return 1;
	}

	// Set brightness level of 8x8 LED display (doesn't seem to make any difference)
	if (extLED8x8DisplayBrightness(15) == false) {
		printf("ERROR!  extLED8x8DisplayBrightness() returned false!\n");
		return 1;
	}

	// Set the rotation of the 8x8 LED display
	extLED8x8SetDisplayRotation(DISPLAY_ROTATE0);

	int k = 0;

	// Cycle thru displaying icons
    while (1) {
		extLED8x8ScrollText("HELLO WORLD 1234567890", font8x8, 50, k);
		nanosleep(&reqDelay, (struct timespec *) NULL);
		if (++k == 4)
			k = 0;
    }

    return 0;
}

