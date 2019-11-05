#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bbg_led.h"
#include "zen_joystick.h"
#include "zen_buzzer.h"
#include "zen_accelerometer.h"
#include "zen_potentiometer.h"
#include "ext_8x8led.h"
#include "ext_buttons.h"
#include "icons8x8.h"
//#include "font8x8.h"
#include "catchGame.h"
#include "simonGame.h"

#define MAX_GAMES 2

int game_selection = 0;
_Bool game_running = false;
int anim_count = 0;
int sample_count = 0;
unsigned char led_brightness;

int main(int argc, char **argv)
{
	int potvalue;

	zenJoystickInit();
	zenBuzzerInit();
	extPushButtonInit();

    struct timespec reqDelay;
    reqDelay.tv_sec = 0;
    reqDelay.tv_nsec = 10000000;

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

	// Set the rotation of the 8x8 LED display
	extLED8x8SetDisplayRotation(DISPLAY_ROTATE0);


	// Main game loop
    while (1) {
		if (game_running == true) {  // A game is running
				// Check for exiting game
			if (extPushButtonPressed(PUSHBUTTON_ENTER_EXIT) == true) {
				switch(game_selection) {
					case 0:  // Simon game
						simonGame_stop();
						break;

					case 1:  // Catch game
						catchGame_stop();
						break;
				}
				game_running = false;
			}
		}

		else {  // Game is not running

				// Display game icon(s)
			switch(game_selection) {
				case 0:  // Simon game
					extLED8x8FillPixel(0);
					extLED8x8LoadImage(&(icons8x8[RIGHT_ARROW_ICON+(anim_count * 8)]));
					extLED8x8DisplayUpdate();
					break;

				case 1:  // Catch game
					extLED8x8FillPixel(0);
					extLED8x8DrawPixel(3, 6, 1);
					extLED8x8DrawPixel(5, 6, 1);
					extLED8x8DrawPixel(3, 7, 1);
					extLED8x8DrawPixel(4, 7, 1);
					extLED8x8DrawPixel(5, 7, 1);
					extLED8x8DrawPixel(4, anim_count+2, 1);
					extLED8x8DisplayUpdate();
					break;
			}

				// Increment animation count
			if (++sample_count == 50) {
				sample_count = 0;
				if (++anim_count == 4)
					anim_count = 0;
			}

				// Check if ENTER is pressed to run game
			if (extPushButtonPressed(PUSHBUTTON_ENTER_EXIT) == true) {
				switch(game_selection) {
					case 0:  // Simon game
						simonGame_start();
						break;

					case 1:  // Catch game
						catchGame_start();
						break;
				}
				game_running = true;
			}

				// Read potentiometer and adjust 8x8 LED brightness
			potvalue = zenPotentiometerReadA2DValue();
			led_brightness = (unsigned char) (potvalue>>8);
			if (extLED8x8DisplayBrightness(led_brightness) == false) {
				printf("ERROR!  extLED8x8DisplayBrightness() returned false!\n");
				return 1;
			}


				// Check if SELECTION is pressed to cycle through game selection
			if (extPushButtonPressed(PUSHBUTTON_SELECT) == true) {
				if (++game_selection == MAX_GAMES)
					game_selection = 0;
			}
		}

		nanosleep(&reqDelay, (struct timespec *) NULL);
    }

    return 0;
}

