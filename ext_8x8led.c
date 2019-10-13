#include "ext_8x8led.h"

//------------ variables and definitions -----------------------
static unsigned char displaybuffer[17];  // 1 command byte + 16 bytes (rows), 8 bits (columns) per byte
static int i2cFileDesc;


//------------ functions ---------------------------------------
//***** static functions ******************************
static void assert_blinktype_OK (enum ht16k33_blink blinktype)
{
    assert((blinktype >= HT16K33_BLINK_OFF) && (blinktype <= HT16K33_BLINK_0_5HZ));
}



//***** public functions ******************************

//*****************************************************
// Initializes the 8x8 LED matrix
// Return value:  true=success, false=fail
//*****************************************************
_Bool extLED8x8Init()
{
	FILE *pipe;
	int exitCode;

	//----------------------------------------------------------------
	// Configure P9_17 and P9_18 pins as I2C using "config-pin" program
	//----------------------------------------------------------------
		// Configure P9_17 as I2C
	pipe = popen("config-pin P9_17 i2c", "r");
	exitCode = WEXITSTATUS(pclose(pipe));
	if (exitCode != 0) {
		printf("Program failed: %d\n", exitCode);
	}

		// Configure P9_18 as I2C
	pipe = popen("config-pin P9_18 i2c", "r");
	exitCode = WEXITSTATUS(pclose(pipe));
	if (exitCode != 0) {
		printf("Program failed: %d\n", exitCode);
	}

	//----------------------------------------------------------------
	// Initialize display buffer
	//----------------------------------------------------------------
	displaybuffer[0] = HT16K33_CMD_DISPRAM;
	for (int k = 1; k < 17; k++)
		displaybuffer[k] = 0;

	//----------------------------------------------------------------
	// Open I2C interface
	//----------------------------------------------------------------
		// Open I2C driver
	i2cFileDesc = open("/dev/i2c-1", O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (/dev/i2c-1)\n");
		return false;
	}

		// Set slave address to 0x70
	int result = ioctl(i2cFileDesc, I2C_SLAVE, 0x70);
	if (result < 0) {
		printf("Unable to set I2C device to slave address to 0x70.\n");
		return false;
	}

	//----------------------------------------------------------------
	// Initialize display RAM
	//----------------------------------------------------------------
	result = write(i2cFileDesc, displaybuffer, 17);
	if (result != 17) {
		printf("ERROR!  Unable to write i2c register\n");
		return false;
	}

	//----------------------------------------------------------------
	// Turn on oscillator
	//----------------------------------------------------------------
	unsigned char buff[2];
	buff[0] = HT16K33_CMD_SYSSETUP | 0x01;
	result = write(i2cFileDesc, buff, 1);
	if (result != 1) {
		printf("ERROR!  Unable to write i2c register\n");
		return false;
	}

	//----------------------------------------------------------------
	// Set max. brightness (0x0F)
	//----------------------------------------------------------------
	buff[0] = HT16K33_CMD_BRIGHTNESS | 0x0F;
	result = write(i2cFileDesc, buff, 1);
	if (result != 1) {
		printf("ERROR!  Unable to write i2c register\n");
		return false;
	}

	//----------------------------------------------------------------
	// Turn on LEDs with no blinking
	//----------------------------------------------------------------
	buff[0] = HT16K33_CMD_DISPSETUP | 0x01;  // Bit 0=1 turns on display, Bits 2-1=0 means no blinking
	result = write(i2cFileDesc, buff, 1);
	if (result != 1) {
		printf("ERROR!  Unable to write i2c register\n");
		return false;
	}

	close(i2cFileDesc);

	return true;  // success
}


//*****************************************************
// Updates the 8x8 LED matrix display RAM (pixel) data
// Return value:  true=success, false=fail
//*****************************************************
_Bool extLED8x8DisplayUpdate()
{
	//----------------------------------------------------------------
	// Open I2C interface
	//----------------------------------------------------------------
		// Open I2C driver
	i2cFileDesc = open("/dev/i2c-1", O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (/dev/i2c-1)\n");
		return false;
	}

		// Set slave address to 0x70
	int result = ioctl(i2cFileDesc, I2C_SLAVE, 0x70);
	if (result < 0) {
		printf("Unable to set I2C device to slave address to 0x70.\n");
		return false;
	}

	//----------------------------------------------------------------
	// Write display RAM
	//----------------------------------------------------------------
	result = write(i2cFileDesc, displaybuffer, 17);
	if (result != 17) {
		printf("ERROR!  Unable to write i2c register\n");
		return false;
	}

	close(i2cFileDesc);

	return true;  // success
}


//*****************************************************
// Turn off the 8x8 LED matrix.
// Return value:  true=success, false=fail
//*****************************************************
_Bool extLED8x8DisplayOff()
{
	//----------------------------------------------------------------
	// Open I2C interface
	//----------------------------------------------------------------
		// Open I2C driver
	i2cFileDesc = open("/dev/i2c-1", O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (/dev/i2c-1)\n");
		return false;
	}

		// Set slave address to 0x70
	int result = ioctl(i2cFileDesc, I2C_SLAVE, 0x70);
	if (result < 0) {
		printf("Unable to set I2C device to slave address to 0x70.\n");
		return false;
	}

	//----------------------------------------------------------------
	// Turn off display
	//----------------------------------------------------------------
	unsigned char buff[2];
	buff[0] = HT16K33_CMD_DISPSETUP;  // Bit0=0 turns off display
	result = write(i2cFileDesc, buff, 1);
	if (result != 1) {
		printf("ERROR!  Unable to write i2c register\n");
		return false;
	}

	close(i2cFileDesc);

	return true;  // success
}


//*****************************************************
// Turn on the 8x8 LED matrix.
// Parameter "blinktype" specifies the blinking type.
// Return value:  true=success, false=fail
//*****************************************************
_Bool extLED8x8DisplayOn(enum ht16k33_blink blinktype)
{
	// Parameters range checking
	assert_blinktype_OK (blinktype);

	//----------------------------------------------------------------
	// Open I2C interface
	//----------------------------------------------------------------
		// Open I2C driver
	i2cFileDesc = open("/dev/i2c-1", O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (/dev/i2c-1)\n");
		return false;
	}

		// Set slave address to 0x70
	int result = ioctl(i2cFileDesc, I2C_SLAVE, 0x70);
	if (result < 0) {
		printf("Unable to set I2C device to slave address to 0x70.\n");
		return false;
	}

	//----------------------------------------------------------------
	// Turn off display
	//----------------------------------------------------------------
	unsigned char buff[2];
	buff[0] = HT16K33_CMD_DISPSETUP | 0x01 | (blinktype<<1);
	result = write(i2cFileDesc, buff, 1);
	if (result != 1) {
		printf("ERROR!  Unable to write i2c register\n");
		return false;
	}

	close(i2cFileDesc);

	return true;  // success
}


//*****************************************************
// Set brightness level of 8x8 LED matrix.
// Parameter "brightness" specifies the brightness level (0 to 15).
// Return value:  true=success, false=fail
//*****************************************************
_Bool extLED8x8DisplayBrightness(unsigned char brightness)
{
	//----------------------------------------------------------------
	// Open I2C interface
	//----------------------------------------------------------------
		// Open I2C driver
	i2cFileDesc = open("/dev/i2c-1", O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (/dev/i2c-1)\n");
		return false;
	}

		// Set slave address to 0x70
	int result = ioctl(i2cFileDesc, I2C_SLAVE, 0x70);
	if (result < 0) {
		printf("Unable to set I2C device to slave address to 0x70.\n");
		return false;
	}

	//----------------------------------------------------------------
	// Turn off display
	//----------------------------------------------------------------
	unsigned char buff[2];
	if (brightness > 15)
		brightness = 15;
	buff[0] = HT16K33_CMD_BRIGHTNESS | brightness;
	result = write(i2cFileDesc, buff, 1);
	if (result != 1) {
		printf("ERROR!  Unable to write i2c register\n");
		return false;
	}

	close(i2cFileDesc);

	return true;  // success
}


//============== Drawing routines ========================================
//*****************************************************
// Fill display buffer with pixel value.
//*****************************************************
void extLED8x8FillPixel(unsigned char pixelval)
{
	for (int k = 1; k < 17; k++) {
		if (pixelval)
			displaybuffer[k] = 0xFF;
		else
			displaybuffer[k] = 0x00;
	}
}


//*****************************************************
// Draw a pixel into the display buffer.
// HT16K33 controller memory is 16x8, but only 8x8 is used.
// Odd bytes of display buffer (rows 9 to 16) are unused.
// Top, left corner pixel coordinate is (0,0).
// Bottom, right corner pixel coordinate is (7,7).
//*****************************************************
void extLED8x8DrawPixel(unsigned int x, unsigned int y, unsigned char pixelval)
{
	// Only draw pixel if it is inbounds (x=0 to 7, y=0 to 7)
	if ((x < 8) && (y < 8)) {
		if (pixelval) {
			displaybuffer[y*2] |= (0x80 >> x);
		}
		else {
			displaybuffer[y*2] &= ~(0x80 >> x);
		}
	}

}


//*****************************************************
// Load an 8x8 image into display buffer.
// Parameter "img" is a pointer to an 8-byte unsigned char array.
// HT16K33 controller memory is 16x8, but only 8x8 is used.
// Odd bytes of display buffer (rows 9 to 16) are unused.
//*****************************************************
void extLED8x8LoadImage(unsigned char *img)
{
	for (int k = 0; k < 8; k++) {
		displaybuffer[k*2] = *img;
		img++;
	}
}


