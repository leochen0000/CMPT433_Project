#include "ext_8x8led.h"

//------------ variables and definitions -----------------------
static unsigned char localbuffer[8];
static unsigned char displaybuffer[17];  // 1 command byte + 16 bytes (rows), 8 bits (columns) per byte
static int i2cFileDesc;
static enum display_rotation disprotation = DISPLAY_ROTATE0;


//------------ functions ---------------------------------------
//***** static functions ******************************
static void assert_blinktype_OK (enum ht16k33_blink blinktype)
{
    assert((blinktype >= HT16K33_BLINK_OFF) && (blinktype <= HT16K33_BLINK_0_5HZ));
}


//*****************************************************
// Remap an 8x8 icon so that it displays correctly on 8x8 LED with rotation.
// "rot" parameter specifies the rotation (0, 90, 180, 270 degrees).
//*****************************************************
static void extLED8x8RemapIcon(unsigned char *img, unsigned char *outimg)
{
	unsigned char tmpbyte;
	unsigned char tmpin;
	int k;
	int j;

	// 270 degrees rotation
	if (disprotation == DISPLAY_ROTATE270) {
		for (k = 0; k < 8; k++) {
			tmpin = img[k];
			if (tmpin & 0x80)
				tmpin = 0x01 | (tmpin << 1);
			else
				tmpin = (tmpin << 1);

			for (j = 0; j < 8; j++) {
				tmpbyte <<= 1;
				if (tmpin & 1)
					tmpbyte |= 0x01;
				tmpin >>= 1;
			}
			outimg[k] = tmpbyte;
		}
	}

	// 0 degrees rotation
	else if (disprotation == DISPLAY_ROTATE0) {
		unsigned char tmpbuff[8];

		for (k = 0; k < 8; k++) {
			for (j = 0; j < 8; j++) {
				tmpbyte <<= 1;
				if (img[j] & (1<<k))
					tmpbyte |= 0x01;
			}
			if (tmpbyte & 0x01)
				tmpbyte = 0x80 | (tmpbyte >> 1);
			else
				tmpbyte = (tmpbyte >> 1);
			tmpbuff[k] = tmpbyte;
		}

		for (k = 0; k < 8; k++) {
			outimg[k] = tmpbuff[7-k];
		}
	}

	// 90 degrees rotation
	else if (disprotation == DISPLAY_ROTATE90) {
		for (k = 0; k < 8; k++) {
			tmpin = img[k];
			if (tmpin & 0x01)
				tmpin = 0x80 | (tmpin >> 1);
			else
				tmpin = (tmpin >> 1);

			outimg[k] = tmpin;
		}

		for (k = 0; k < 4; k++) {
			tmpbyte = outimg[k];
			outimg[k] = outimg[7-k];
			outimg[7-k] = tmpbyte;
		}
	}

	// 180 degrees rotation
	else {
		unsigned char tmpbuff[8];

		for (k = 0; k < 8; k++) {
			for (j = 0; j < 8; j++) {
				tmpbyte >>= 1;
				if (img[j] & (1<<k))
					tmpbyte |= 0x80;
			}
			if (tmpbyte & 0x01)
				tmpbyte = 0x80 | (tmpbyte >> 1);
			else
				tmpbyte = (tmpbyte >> 1);
			tmpbuff[k] = tmpbyte;
		}

		for (k = 0; k < 8; k++) {
			outimg[k] = tmpbuff[7-k];
		}
	}

}


//***** public functions ******************************

//*****************************************************
// Initializes the 8x8 LED matrix
// Return value:  true=success, false=fail
//*****************************************************
_Bool extLED8x8Init()
{
	//----------------------------------------------------------------
	// Initialize display buffer
	//----------------------------------------------------------------
	displaybuffer[0] = HT16K33_CMD_DISPRAM;
	for (int k = 0; k < 8; k++)
		localbuffer[k] = 0;
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
	// Remap local buffer to display buffer for sending
	//----------------------------------------------------------------
	unsigned char tmpbuff[8];
	extLED8x8RemapIcon(localbuffer, tmpbuff);
	for (int k = 0; k < 8; k++) {
		displaybuffer[1+k*2] = tmpbuff[k];
	}

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
// Set rotation of 8x8 display.  0, 90, 180, or 270 clockwise.
//*****************************************************
void extLED8x8SetDisplayRotation(enum display_rotation rotval)
{
	disprotation = rotval;
}


//*****************************************************
// Fill local buffer with pixel value.
//*****************************************************
void extLED8x8FillPixel(unsigned char pixelval)
{
	for (int k = 0; k < 8; k++) {
		if (pixelval)
			localbuffer[k] = 0xFF;
		else
			localbuffer[k] = 0x00;
	}
}


//*****************************************************
// Draw a pixel into the local buffer.
// Top, left corner pixel coordinate is (0,0).
// Bottom, right corner pixel coordinate is (7,7).
//*****************************************************
void extLED8x8DrawPixel(unsigned int x, unsigned int y, unsigned char pixelval)
{
	// Only draw pixel if it is inbounds (x=0 to 7, y=0 to 7)
	if ((x < 8) && (y < 8)) {
		if (pixelval) {
			localbuffer[y] |= (0x80 >> x);
		}
		else {
			localbuffer[y] &= ~(0x80 >> x);
		}
	}

}


//*****************************************************
// Load an 8x8 image into local buffer.
//*****************************************************
void extLED8x8LoadImage(unsigned char *img)
{
	for (int k = 0; k < 8; k++) {
		localbuffer[k] = img[k];
	}
}


