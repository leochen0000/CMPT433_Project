// ext_8x8led.h
// Routines/definitions for external 8x8 LED display

#ifndef EXT_8X8LED_H
#define EXT_8X8LED_H

#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>


//------- Enums and definitions --------------------
#define HT16K33_CMD_DISPRAM           0x00
#define HT16K33_CMD_SYSSETUP          0x20
#define HT16K33_CMD_DISPSETUP         0x80
#define HT16K33_CMD_BRIGHTNESS        0xE0

enum ht16k33_blink
{
	HT16K33_BLINK_OFF = 0,
	HT16K33_BLINK_2HZ,
	HT16K33_BLINK_1HZ,
	HT16K33_BLINK_0_5HZ
};

enum display_rotation
{
	DISPLAY_ROTATE0 = 0,
	DISPLAY_ROTATE90,
	DISPLAY_ROTATE180,
	DISPLAY_ROTATE270
};

enum scroll_direction
{
	SCROLL_UP = 0,
	SCROLL_DOWN,
	SCROLL_LEFT,
	SCROLL_RIGHT
};


//------- function prototypes ----------------------
	// Display routines
//*****************************************************
// Initializes the 8x8 LED matrix
// Return value:  true=success, false=fail
//*****************************************************
_Bool extLED8x8Init();

//*****************************************************
// Updates the 8x8 LED matrix display RAM (pixel) data
// Return value:  true=success, false=fail
//*****************************************************
_Bool extLED8x8DisplayUpdate();

//*****************************************************
// Turn off the 8x8 LED matrix.
// Return value:  true=success, false=fail
//*****************************************************
_Bool extLED8x8DisplayOff();

//*****************************************************
// Turn on the 8x8 LED matrix.
// Parameter "blinktype" specifies the blinking type.
// Return value:  true=success, false=fail
//*****************************************************
_Bool extLED8x8DisplayOn(enum ht16k33_blink blinktype);

//*****************************************************
// Set brightness level of 8x8 LED matrix.
// Parameter "brightness" specifies the brightness level (0 to 15).
// Return value:  true=success, false=fail
//*****************************************************
_Bool extLED8x8DisplayBrightness(unsigned char brightness);


	// Drawing routines
//*****************************************************
// Set rotation of 8x8 display.  0, 90, 180, or 270 clockwise.
//*****************************************************
void extLED8x8SetDisplayRotation(enum display_rotation rotval);

//*****************************************************
// Fill local buffer with pixel value.
//*****************************************************
void extLED8x8FillPixel(unsigned char pixelval);

//*****************************************************
// Draw a pixel into the local buffer.
// Top, left corner pixel coordinate is (0,0).
// Bottom, right corner pixel coordinate is (7,7).
//*****************************************************
void extLED8x8DrawPixel(unsigned int x, unsigned int y, unsigned char pixelval);

//*****************************************************
// Load an 8x8 image into local buffer.
//*****************************************************
void extLED8x8LoadImage(unsigned char *img);  // img is pointer to 8-byte unsigned char array

//*****************************************************
// Scroll 7-bit ASCII text one character at a time through the 8x8 matrix
// txtstr:  text string to display
// fontset:  128-character 8x8 font set
// scrollmsdelay:  pixel shift delay in milliseconds
// scrolldir:  scroll direction
//*****************************************************
void extLED8x8ScrollText(char *txtstr, unsigned char *fontset, int scrollmsdelay, enum scroll_direction scrolldir);

//*****************************************************
// Countdown 3,2,1
//*****************************************************
void extLED8x8CountDown321(unsigned char *font);

//*****************************************************
// LED logo for exiting a game
//*****************************************************
void extLED8x8ExitGame(unsigned char *font);

//*****************************************************
// Get local display buffer data
//*****************************************************
void extLED8x8GetLocalBuffer(unsigned char *font);

#endif

