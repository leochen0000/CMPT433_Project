// miniGamesConsole.c
// Main program for "Mini Games Console" project.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bbg_led.h"
#include "zen_joystick.h"
#include "zen_buzzer.h"
#include "zen_accelerometer.h"
#include "zen_potentiometer.h"
#include "zen_segdisplay.h"
#include "ext_8x8led.h"
#include "ext_buttons.h"
#include "icons8x8.h"
#include "font8x8.h"
#include "udpServer1.h"
#include "balanceGame.h"
#include "catchGame.h"
#include "simonGame.h"
#include "snakeGame.h"
#include "floppyBirbGame.h"

#define MAX_GAMES 5

int anim_count = 0;
int sample_count = 0;

_Bool game_running = false;
int game_selection = 0;
unsigned char led_brightness;
char *(gamename[]) = {
	"SIMON",
	"CATCH",
	"BALANCE",
	"SNAKE",
	"FLOPPY BIRB"
};


int main(int argc, char **argv)
{
	int potvalue;

	zenSegDisplayInit();
	zenSegDisplayStart();
	zenAccelerometerInit();
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

	// Start UDP listener
	UDPServer_startListening();

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

					case 2:  // Balance game
						balanceGame_stop();
						break;

					case 3:  // Snake game
						snakeGame_stop();
						break;

					case 4:  // Floppy Birb game
						floppyBirbGame_stop();
						break;
				}
				game_running = false;
			}
		}

		else {  // Game is not running

				// Display game icon(s)
			switch(game_selection) {
				case 0:  // Simon game
					zenSegDisplayUpdateNum(0); // Game #0
					extLED8x8FillPixel(0);
					if (anim_count < 3)
						extLED8x8LoadImage(&(icons8x8[RIGHT_ARROW_ICON+(anim_count * 8)]));
					else
						extLED8x8LoadImage(&(icons8x8[RIGHT_ARROW_ICON+(3 * 8)]));
					extLED8x8DisplayUpdate();
					break;

				case 1:  // Catch game
					zenSegDisplayUpdateNum(1); // Game #1
					extLED8x8FillPixel(0);
					extLED8x8DrawPixel(3, 6, 1);
					extLED8x8DrawPixel(5, 6, 1);
					extLED8x8DrawPixel(3, 7, 1);
					extLED8x8DrawPixel(4, 7, 1);
					extLED8x8DrawPixel(5, 7, 1);
					if (anim_count < 4)
						extLED8x8DrawPixel(4, anim_count+2, 1);
					else
						extLED8x8DrawPixel(4, 6, 1);
					extLED8x8DisplayUpdate();
					break;

				case 2:  // Balance game
					zenSegDisplayUpdateNum(2); // Game #1
					extLED8x8FillPixel(0);
					if (anim_count == 0)
						extLED8x8DrawPixel(3, 3, 1);
					else if (anim_count == 1)
						extLED8x8DrawPixel(4, 4, 1);
					else if (anim_count == 2)
						extLED8x8DrawPixel(5, 4, 1);
					else if (anim_count == 3)
						extLED8x8DrawPixel(5, 3, 1);
					else if (anim_count == 4)
						extLED8x8DrawPixel(4, 3, 1);
					else
						extLED8x8DrawPixel(3, 4, 1);
					extLED8x8DisplayUpdate();
					break;

				case 3:  // Snake game
					zenSegDisplayUpdateNum(3); // Game #3
					extLED8x8FillPixel(0);
					if (anim_count == 0) {
						extLED8x8DrawPixel(3, 3, 1);
						extLED8x8DrawPixel(4, 3, 1);
						extLED8x8DrawPixel(5, 3, 1);
						extLED8x8DrawPixel(6, 3, 1);
					}
					else if (anim_count == 1) {
						extLED8x8DrawPixel(5, 3, 1);
						extLED8x8DrawPixel(6, 3, 1);
						extLED8x8DrawPixel(6, 4, 1);
						extLED8x8DrawPixel(6, 5, 1);
					}
					else if (anim_count == 2) {
						extLED8x8DrawPixel(6, 4, 1);
						extLED8x8DrawPixel(6, 5, 1);
						extLED8x8DrawPixel(5, 5, 1);
						extLED8x8DrawPixel(4, 5, 1);
					}
					else if (anim_count == 3) {
						extLED8x8DrawPixel(5, 5, 1);
						extLED8x8DrawPixel(4, 5, 1);
						extLED8x8DrawPixel(3, 5, 1);
						extLED8x8DrawPixel(3, 4, 1);
					}
					else if (anim_count == 4) {
						extLED8x8DrawPixel(3, 5, 1);
						extLED8x8DrawPixel(3, 4, 1);
						extLED8x8DrawPixel(3, 3, 1);
						extLED8x8DrawPixel(4, 3, 1);
					}
					else {
						extLED8x8DrawPixel(3, 4, 1);
						extLED8x8DrawPixel(3, 3, 1);
						extLED8x8DrawPixel(4, 3, 1);
						extLED8x8DrawPixel(5, 3, 1);
					}
					extLED8x8DisplayUpdate();
					break;

				case 4:  // Floppy Birb game
					zenSegDisplayUpdateNum(4); // Game #4
					extLED8x8FillPixel(0);
					if (anim_count == 0) {
						extLED8x8DrawPixel(1, 4, 1);
						extLED8x8DrawPixel(2, 3, 1);
					}
					else if (anim_count == 1) {
						extLED8x8DrawPixel(2, 3, 1);
						extLED8x8DrawPixel(3, 3, 1);
					}
					else if (anim_count == 2) {
						extLED8x8DrawPixel(3, 3, 1);
						extLED8x8DrawPixel(4, 4, 1);
					}
					else if (anim_count == 3) {
						extLED8x8DrawPixel(4, 4, 1);
						extLED8x8DrawPixel(5, 3, 1);
					}
					else if (anim_count == 4) {
						extLED8x8DrawPixel(5, 3, 1);
						extLED8x8DrawPixel(6, 3, 1);
					}
					extLED8x8DisplayUpdate();
					break;
			}

				// Increment animation count
			if (++sample_count == 25) {
				sample_count = 0;
				if (++anim_count == 6)
					anim_count = 0;
			}

				// Check if ENTER is pressed to run game
			if (extPushButtonPressed(PUSHBUTTON_ENTER_EXIT) == true) {
				game_running = true;
				switch(game_selection) {
					case 0:  // Simon game
						simonGame_start();
						break;

					case 1:  // Catch game
						catchGame_start();
						break;

					case 2:  // Balance game
						balanceGame_start();
						break;

					case 3:  // Snake game
						snakeGame_start();
						break;

					case 4:  // Floppy Birb game
						floppyBirbGame_start();
						break;
				}
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

				printf("'%s' game selected\n", gamename[game_selection]);
			}
		}

		nanosleep(&reqDelay, (struct timespec *) NULL);
    }

    return 0;
}

