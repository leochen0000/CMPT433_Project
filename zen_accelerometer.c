#include "zen_accelerometer.h"

//------------ variables and definitions -----------------------
static int i2cFileDesc;


//------------ functions ---------------------------------------
//***** static functions ******************************


//***** public functions ******************************

//*****************************************************
// Initializes Zen Cape accelerometer.
// Return value:  true=success, false=fail
//*****************************************************
_Bool zenAccelerometerInit()
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
	// Initialize I2C interface and enable accelerometer.
	//----------------------------------------------------------------
		// Open I2C driver
	i2cFileDesc = open("/dev/i2c-1", O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (/dev/i2c-1)\n");
		return false;
	}

		// Set slave address to 0x1C
	int result = ioctl(i2cFileDesc, I2C_SLAVE, 0x1C);
	if (result < 0) {
		printf("Unable to set I2C device to slave address to 0x1C.\n");
		return false;
	}

		// Set register 0x2A to 0x04 and then 0x05
	unsigned char buff[2];
	buff[0] = 0x2A;
	buff[1] = 0x04;
	result = write(i2cFileDesc, buff, 2);
	if (result != 2) {
		printf("ERROR!  Unable to write i2c register\n");
		return false;
	}

	buff[1] = 0x05;
	result = write(i2cFileDesc, buff, 2);
	if (result != 2) {
		printf("ERROR!  Unable to write i2c register\n");
		return false;
	}

	close(i2cFileDesc);

	return true;  // success
}


//*****************************************************
// Read X, Y, Z values from Zen Cape accelerometer.
// Return value:  true=success, false=fail
//*****************************************************
_Bool zenAccelerometerRead(int *xval, int *yval, int *zval)
{
		// Open I2C driver
	i2cFileDesc = open("/dev/i2c-1", O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (/dev/i2c-1)\n");
		return false;
	}

		// Set slave address to 0x1C
	int result = ioctl(i2cFileDesc, I2C_SLAVE, 0x1C);
	if (result < 0) {
		printf("Unable to set I2C device to slave address to 0x1C.\n");
		return false;
	}

		// Set register to 0x00
	unsigned char buff[8];
	buff[0] = 0x00;
	result = write(i2cFileDesc, buff, 1);
	if (result != 1) {
		printf("ERROR!  Unable to read i2c register\n");
		return false;
	}

	result = read(i2cFileDesc, buff, 7);  // Read 7 bytes status,XMSB,XLSB,YMSB,YLSB,ZMSB,ZLSB
	if (result != 7) {
		printf("ERROR!  Unable to read i2c register\n");
		return false;
	}

	// Form X, Y, Z values
	*xval = (((int) buff[1]) << 8) + buff[2];
	*xval >>= 4;
	if (*xval > 2047)
		*xval -= 4096;

	*yval = (((int) buff[3]) << 8) + buff[4];
	*yval >>= 4;
	if (*yval > 2047)
		*yval -= 4096;

	*zval = (((int) buff[5]) << 8) + buff[6];
	*zval >>= 4;
	if (*zval > 2047)
		*zval -= 4096;

	close(i2cFileDesc);

	return true;  // success
}


