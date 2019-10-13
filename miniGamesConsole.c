#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bbg_led.h"
#include "zen_joystick.h"
#include "zen_accelerometer.h"
#include "ext_buttons.h"
#include "ext_photoresistor.h"

int main(int argc, char **argv)
{
    struct timespec reqDelay;
    reqDelay.tv_sec = 0;
    reqDelay.tv_nsec = 100000000;  // 100ms = 100,000us = 100,000,000ns

    while (1) {
		printf("ADC1 = %d\n", extPhotoresistorReadA2DValue());
		nanosleep(&reqDelay, (struct timespec *) NULL);

		if (extPushButtonReleased (PUSHBUTTON_LEFT)) {
			printf("%s\n", extPushButtonName(PUSHBUTTON_LEFT));
		}
		if (extPushButtonReleased (PUSHBUTTON_MIDDLE)) {
			printf("%s\n", extPushButtonName(PUSHBUTTON_MIDDLE));
		}
		if (extPushButtonReleased (PUSHBUTTON_RIGHT)) {
			printf("%s\n", extPushButtonName(PUSHBUTTON_RIGHT));
		}

    }

    return 0;
}

