// ext_8x8led.h
// Routines/definitions for external 8x8 LED display

#ifndef EXT_8X8LED_H
#define EXT_8X8LED_H

#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
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

//------- function prototypes ----------------------
	// Display routines
_Bool extLED8x8Init();
_Bool extLED8x8DisplayUpdate();
_Bool extLED8x8DisplayOff();
_Bool extLED8x8DisplayOn(enum ht16k33_blink blinktype);
_Bool extLED8x8DisplayBrightness(unsigned char brightness);

	// Drawing routines
void extLED8x8FillPixel(unsigned char pixelval);
void extLED8x8DrawPixel(unsigned int x, unsigned int y, unsigned char pixelval);
void extLED8x8LoadImage(unsigned char *img);  // img is pointer to 8-byte unsigned char array

#endif

